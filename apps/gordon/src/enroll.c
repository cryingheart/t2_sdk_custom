/*

                +--------+
                | random |                     .'
                +--------+                    /
                    |       +------+        .'
                    +------>| pamp |--> S  /
                    |       +------+     .'
                +--------+              /
                | encode |            .'         Reconstruct
                +--------+   ,-----. /
                    |       (       )
    +---------+    ,-.      |`-----'|       ,-.      +---------+
    | measure |-->( + )---> |       | ---->( + )<----| measure'|
    +---------+    `-'      |  HDA  |       `-'      +---------+
                            |       |        |
                           . `-----'     +--------+
          Enrollment      /              | decode |
                        .'               +--------+
                       /                     |
                     .'                   +------+
                    /                     | pamp |--> S
                  .'                      +------+



*/

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <kernel/os.h>
#include <kernel/io.h>
#include <kernel/hwreg.h>
#include <kernel/callout.h>
#include <kernel/cipher.h>
#include <kernel/spi-mem.h>
#include <kernel/secureboot.h>
#include <kernel/efuse.h>
#include <kernel/page_alloc.h>
#include <kernel/wfe.h>
#include "gordon.h"

#ifndef EFUSE_SUPPORT
#define EFUSE_SUPPORT 1
#endif

uint32_t *sram = (uint32_t*)0x70000;

/*************************************/
/* cipher functions                  */
/*************************************/
static __always_inline
void wfe_cipher(void)
{
    wfe_wait(WFE_HANDLER_RAW_CIPHER_IDLE);
}

static void __unused
cipher_ecb(uint32_t *key, uint32_t *input, size_t len, uint32_t *output)
{
    struct ci_hdr hdr = { .tag = CI_TAG };
    struct dma_desc desc[4];

    hdr.ctrl = CI_AES_ECBENC_256;
    hdr.len  = len - 1;

    dma_map_buffer(&desc[0],  &hdr, sizeof hdr, DESC_TX); dma_link(&desc[0], &desc[1]);
    dma_map_buffer(&desc[1],   key,         32, DESC_TX); dma_link(&desc[1], &desc[2]);
    dma_map_buffer(&desc[2], input,        len, DESC_TX); dma_link(&desc[2], NULL);
    dma_start_desc(&hw_cipher->dma_ctrl[0], &desc[0]);

    dma_map_buffer(&desc[3], output, len, DESC_RX); dma_link(&desc[3], NULL);
    dma_start_desc(&hw_cipher->dma_ctrl[1], &desc[3]);
    wfe_cipher();
}

static void __unused
cipher_ecb_128(uint32_t *key, uint32_t *input, size_t len, uint32_t *output, uint32_t inv)
{
    struct ci_hdr hdr = { .tag = CI_TAG };
    struct dma_desc desc[4];

    hdr.ctrl = CI_AES_ECBENC_128;
    hdr.len  = len - 1;

    if (inv)
	hdr.ctrl |= CI_ECB_DEC;

    dma_map_buffer(&desc[0],  &hdr, sizeof hdr, DESC_TX); dma_link(&desc[0], &desc[1]);
    dma_map_buffer(&desc[1],   key,         16, DESC_TX); dma_link(&desc[1], &desc[2]);
    dma_map_buffer(&desc[2], input,        len, DESC_TX); dma_link(&desc[2], NULL);
    dma_start_desc(&hw_cipher->dma_ctrl[0], &desc[0]);

    dma_map_buffer(&desc[3], output, len, DESC_RX); dma_link(&desc[3], NULL);
    dma_start_desc(&hw_cipher->dma_ctrl[1], &desc[3]);

    wfe_cipher();
}

static void
cipher_gf2_mul(uint8_t *data)
{
    uint8_t carry = 0;

    for(int i=0; i<16; i++) {
	uint32_t tmp = 2*data[i] + carry;
	data[i] = tmp;
	carry = tmp>>8;
    }

    if (carry)
	data[0] ^= 0x87;
}

static void
cipher_xor(uint8_t *r, uint8_t *d1, uint8_t *d2)
{
    for(int i=0; i<16; i++) {
	*r++ = *d1++ ^ *d2++;
    }
}

static void __unused
cipher_xex_soft(uint32_t *key, uint32_t sector, uint8_t *data, size_t len, uint32_t inv)
{
    uint32_t k[4] = {sector, 0};
    uint32_t tmp[4];

    cipher_ecb_128(&key[4], k, 16, k, 0);

    while(len) {
	cipher_xor((uint8_t*)tmp, data, (uint8_t*)k);
	cipher_ecb_128(&key[0], tmp, 16, tmp, inv);
	cipher_xor(data, (uint8_t*)tmp, (uint8_t*)k);
	cipher_gf2_mul((uint8_t*)k);
	data += 16;
	len -=16;
    }
}

int
puf_enroll(struct secure_boot *bb, uint32_t *secret)
{
    /* power cycle sram, and use the offtime to destill secret */
    mmio_wr16_set(PMU_PD_RAM_3, &hw_pmu->buck.pd);
    os_wait_usec(100*1000);
    mmio_wr16_clr(PMU_PD_RAM_3, &hw_pmu->buck.pd);
    os_wait_usec(100*1000);

    // encode secret, and generate hda
    puf_enroll_encode(264, secret, sram, bb->hda);

    return 0;
}


int
enroll(struct secure_boot *bb, uint8_t mode, const uint8_t *secret,
       const uint8_t *public_key, const uint8_t *cipher_key)
{
    uint32_t sb_secret[8];
    uint32_t fuse[8] = {0};
    uint32_t secret32[9];

    os_printf("Secureboot enrollment\n");

    fuse[0] = os_efuse_read(0);

    if((fuse[0] & SECURE_BOOT_MODE_MASK) != SECURE_BOOT_MODE_NORMAL) {
	os_printf("Fuses already programmed.\n");
        return -EEXIST;
    }

    if (mode & 1) {
	os_printf("  using puf for secret\n");
	memcpy(secret32, secret, 36);
	puf_enroll(bb, secret32);
	fuse[0] = SECURE_BOOT_MODE_PUF;
    }
    if (mode & 2) {
	os_printf("  using fuse for secret\n");
	memcpy(&secret32[0], secret, 16);
	memcpy(&secret32[4], secret, 16);
	memcpy(&fuse[4], secret, 16);
	fuse[0] = SECURE_BOOT_MODE_FUSE;
    }

    if (mode & 4) {
	memset(bb->fuse, 0, sizeof(bb->fuse));

	if (mode&8) {
            if(EFUSE_SUPPORT) {
                os_printf("  programming fuses\n");
                os_printf("    secureboot mode\n");
                os_efuse_write(0, fuse[0]);
                if (mode&2) {
                    os_printf("    encryption key\n");
                    os_efuse_write(128+00, fuse[4]);
                    os_efuse_write(128+32, fuse[5]);
                    os_efuse_write(128+64, fuse[6]);
                    os_efuse_write(128+96, fuse[7]);
                }
            } else {
                os_printf("  fuse support not enabled\n");
                return -ENXIO;
            }
	} else {
            os_printf("  not actually programming fuses\n");
        }
    } else {
        /* this is for emulated secureboot mode */
        os_printf("  storing fuse content in flash\n");
	memcpy(bb->fuse, fuse, sizeof(bb->fuse));
    }

    memcpy(bb->ecdsa_pub, public_key, 64);
    memcpy(bb->enc_key,   cipher_key, 32);

    cipher_ecb(secret32, secret32, 32, sb_secret);

    cipher_xex_soft(sb_secret, offsetof(struct secure_boot, ecdsa_pub), bb->ecdsa_pub, 64, 1);
    cipher_xex_soft(sb_secret, offsetof(struct secure_boot, enc_key),   bb->enc_key,   32, 1);

    return 0;
}

void os_early_alloc_hook()
{
    os_page_claim((void*)0x70000, (void*)0x80000);
}
/* vim: sw=4 ts=8 expandtab
 */
