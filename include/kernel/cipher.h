#pragma once
#include <kernel/dma.h>

struct ci_hdr {
    uint16_t tag;
#define CI_TAG            0x4349
    uint16_t len;
    uint16_t ctrl;
#define CI_KEY_NONE       (CI_NOLOAD_KEY_64  |  0u)
#define CI_KEY_64         (CI_NOLOAD_KEY_64  |  7u)
#define CI_KEY_128        (CI_NOLOAD_KEY_128 | 15u)
#define CI_KEY_192        (CI_NOLOAD_KEY_192 | 23u)
#define CI_KEY_256        (CI_NOLOAD_KEY_256 | 31u)
#define CI_KEY_2x128      (CI_NOLOAD_KEY_128 | 31u)

#define CI_PARSER         (0u << 5)
#define CI_BLOCK          (1u << 5)
#define CI_STREAM         (2u << 5)
#define CI_BIGNUM         ((3u << 5) | 3)

#define CI_MODE_CNTR      (0u << 7)
#define CI_MODE_GF        (1u << 7)
#define CI_MODE_OFB       (2u << 7)
#define CI_MODE_ECB       (3u << 7)
#define CI_MODE_CBC_MAC   (4u << 7)
#define CI_MODE_CBC_ENC   (5u << 7)
#define CI_MODE_CBC_DEC   (6u << 7)
#define CI_MODE_XEX       (7u << 7)

#define CI_MODE_SHA1      (0u << 7)
#define CI_MODE_SHA2      (1u << 7)

#define CI_CORE_AES       (0u << 10)
#define CI_CORE_TDES      (1u << 10)
#define CI_CORE_SMS4      (2u << 10)
#define CI_CORE_GF        (3u << 10)

#define CI_CORE_RC4       (0u << 10)
#define CI_CORE_MICHAEL   (1u << 10)
#define CI_CORE_CRC32     (2u << 10)
#define CI_CORE_SHA       (3u << 10)

#define CI_NOLOAD_KEY_64  (0u << 12)
#define CI_NOLOAD_KEY_128 (1u << 12)
#define CI_NOLOAD_KEY_192 (2u << 12)
#define CI_NOLOAD_KEY_256 (3u << 12)

#define CI_KEEP_STATE     (1u << 14)
#define CI_ECB_DEC        (1u << 15)
};

#define CI_SHA1             (CI_STREAM | CI_CORE_SHA     | CI_MODE_SHA1 | CI_KEY_NONE)
#define CI_SHA224           (CI_STREAM | CI_CORE_SHA     | CI_MODE_SHA2 | CI_KEY_NONE)
#define CI_SHA256           (CI_STREAM | CI_CORE_SHA     | CI_MODE_SHA2 | CI_KEY_NONE)
#define CI_MICHAEL          (CI_STREAM | CI_CORE_MICHAEL                | CI_KEY_64  )
#define CI_CRC32            (CI_STREAM | CI_CORE_CRC32                  | CI_KEY_NONE)
#define CI_RC4_64           (CI_STREAM | CI_CORE_RC4                    | CI_KEY_64  )
#define CI_RC4_128          (CI_STREAM | CI_CORE_RC4                    | CI_KEY_128 )
#define CI_GHASH            (CI_BLOCK  | CI_CORE_GF      | CI_MODE_GF   | CI_KEY_128 )
#define CI_XEX_128          (CI_BLOCK  | CI_CORE_AES     | CI_MODE_XEX  | CI_KEY_128 )
#define CI_XEX_256          (CI_BLOCK  | CI_CORE_AES     | CI_MODE_XEX  | CI_KEY_256 )

#define CI_AES_CBCMAC_128   (CI_BLOCK | CI_MODE_CBC_MAC | CI_CORE_AES | CI_KEY_128)
#define CI_AES_CBCMAC_192   (CI_BLOCK | CI_MODE_CBC_MAC | CI_CORE_AES | CI_KEY_192)
#define CI_AES_CBCMAC_256   (CI_BLOCK | CI_MODE_CBC_MAC | CI_CORE_AES | CI_KEY_256)

#define CI_AES_CNTR_128     (CI_BLOCK | CI_MODE_CNTR | CI_CORE_AES | CI_KEY_128)
#define CI_AES_CNTR_192     (CI_BLOCK | CI_MODE_CNTR | CI_CORE_AES | CI_KEY_192)
#define CI_AES_CNTR_256     (CI_BLOCK | CI_MODE_CNTR | CI_CORE_AES | CI_KEY_256)

#define CI_AES_OFB_128      (CI_BLOCK | CI_MODE_OFB | CI_CORE_AES | CI_KEY_128)
#define CI_AES_OFB_192      (CI_BLOCK | CI_MODE_OFB | CI_CORE_AES | CI_KEY_192)
#define CI_AES_OFB_256      (CI_BLOCK | CI_MODE_OFB | CI_CORE_AES | CI_KEY_256)

#define CI_AES_ECBENC_128   (CI_BLOCK | CI_MODE_ECB  | CI_CORE_AES | CI_KEY_128)
#define CI_AES_ECBENC_192   (CI_BLOCK | CI_MODE_ECB  | CI_CORE_AES | CI_KEY_192)
#define CI_AES_ECBENC_256   (CI_BLOCK | CI_MODE_ECB  | CI_CORE_AES | CI_KEY_256)

#define CI_AES_ECBDEC_128   (CI_BLOCK | CI_MODE_ECB  | CI_CORE_AES | CI_KEY_128 | CI_ECB_DEC)
#define CI_AES_ECBDEC_192   (CI_BLOCK | CI_MODE_ECB  | CI_CORE_AES | CI_KEY_192 | CI_ECB_DEC)
#define CI_AES_ECBDEC_256   (CI_BLOCK | CI_MODE_ECB  | CI_CORE_AES | CI_KEY_256 | CI_ECB_DEC)

#define CI_AES_CBCENC_128   (CI_BLOCK | CI_MODE_CBC_ENC | CI_CORE_AES | CI_KEY_128)
#define CI_AES_CBCENC_192   (CI_BLOCK | CI_MODE_CBC_ENC | CI_CORE_AES | CI_KEY_192)
#define CI_AES_CBCENC_256   (CI_BLOCK | CI_MODE_CBC_ENC | CI_CORE_AES | CI_KEY_256)

#define CI_AES_CBCDEC_128   (CI_BLOCK | CI_MODE_CBC_DEC | CI_CORE_AES | CI_KEY_128)
#define CI_AES_CBCDEC_192   (CI_BLOCK | CI_MODE_CBC_DEC | CI_CORE_AES | CI_KEY_192)
#define CI_AES_CBCDEC_256   (CI_BLOCK | CI_MODE_CBC_DEC | CI_CORE_AES | CI_KEY_256)

#define CI_TDES_CTR         (CI_BLOCK | CI_MODE_CTR | CI_CORE_TDES | CI_KEY_192)
#define CI_TDES_OFB         (CI_BLOCK | CI_MODE_OFB | CI_CORE_TDES | CI_KEY_192)
#define CI_TDES_CBC_ENC     (CI_BLOCK | CI_MODE_CBC_ENC | CI_CORE_TDES | CI_KEY_192)
#define CI_TDES_CBC_DEC     (CI_BLOCK | CI_MODE_CBC_DEC | CI_CORE_TDES | CI_KEY_192)

struct ci_dma_hdr {
    struct dma_desc desc;
    struct ci_hdr   hdr;
};

static inline void
ci_init_hdr(struct ci_dma_hdr *h, uint16_t ctrl, size_t len)
{
    h->hdr.tag = CI_TAG;
    h->hdr.ctrl = ctrl;
    h->hdr.len = len - 1;

    h->desc.buffer = &h->hdr;
    h->desc.count = sizeof h->hdr;
    h->desc.actual = 0;
    h->desc.flags = DESC_TX;
}

struct os_msg_cipher;

typedef void (*os_cipher_callback_t)(struct os_msg_cipher*);

struct os_msg_cipher {
    struct os_msg         msg;
    os_cipher_callback_t  callback;
    void                 *arg;
    struct dma_desc      *input;
    struct dma_desc      *input2; /* bignum */
    struct dma_desc      *output;
};

static inline void
os_cipher_msg_init(struct os_msg_cipher *cmsg)
{
    cmsg->msg.msg_type = OS_MSG_CIPHER;
    cmsg->callback = NULL;
    cmsg->arg = NULL;
    cmsg->input = NULL;
    cmsg->input2 = NULL;
    cmsg->output = NULL;
}

static inline void
os_cipher_msg_set_input(struct os_msg_cipher *cmsg,
                        struct dma_desc *desc)
{
    cmsg->input = desc;
}

static inline void
os_cipher_msg_set_input2(struct os_msg_cipher *cmsg,
                         struct dma_desc *desc)
{
    cmsg->input2 = desc;
}

static inline void
os_cipher_msg_set_output(struct os_msg_cipher *cmsg,
                         struct dma_desc *desc)
{
    cmsg->output = desc;
}

static inline void
os_cipher_msg_set_callback(struct os_msg_cipher *cmsg,
                           os_cipher_callback_t callback,
                           void *arg)
{
    cmsg->callback = callback;
    cmsg->arg = arg;
}

/* BEGIN GENERATED PROTOS */

bool os_cipher_acquire(bool nowait);

void os_cipher_enqueue(struct os_msg_cipher* cmsg);

void os_cipher_enqueue_sendmsg(struct os_msg_cipher* cmsg);

void
os_cipher_enqueue_simple(struct dma_desc* input,
                         struct dma_desc* output);

void os_cipher_enqueue_sync(struct os_msg_cipher* cmsg);

void os_cipher_init(void *);

void os_cipher_release(void);

/* END GENERATED PROTOS */
