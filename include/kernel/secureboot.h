#pragma once
#include <sha2.h>

#include <kernel/bootsector.h>

#define SECURE_BOOT_DISABLE_JTAG (1<<0)
#define SECURE_BOOT_MODE_MASK    (3<<1)
#define SECURE_BOOT_MODE_NORMAL  (0<<1)
#define SECURE_BOOT_MODE_PUF     (1<<1)
#define SECURE_BOOT_MODE_FUSE    (2<<1)

struct secureboot_context {
    uint32_t   fw_cipher_key[8];
    uint8_t    signature[64];
    uint8_t    ecdsa_public_key[64];
    SHA256_CTX sha256_ctx;
};

extern uint32_t secureboot_secret[9];

bool secureboot_init(struct secureboot_context *ctx, const uint32_t *fuse);
void secureboot_hash_add(struct secureboot_context *ctx, uint32_t addr, size_t len);
int  secureboot_hash_verify(struct secureboot_context *ctx);
void secureboot_get_from_vault(uint8_t *dest, uint32_t offset, uint32_t size);
void secureboot_decrypt(struct secureboot_context *ctx, uint32_t addr, size_t len);
int  secureboot_puf_reconstruct(uint32_t *secret, uint32_t a0, uint32_t a1);
int  puf_reconstruct(uint32_t size, uint32_t *secret, const uint32_t *ram, const uint32_t *hda);
void puf_enroll_encode(uint32_t size, const uint32_t *secret, const uint32_t *ram, uint32_t *hda);
