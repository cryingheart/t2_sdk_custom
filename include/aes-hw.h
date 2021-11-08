#pragma once
#include <stdint.h>
#include <kernel/dma.h>
#include <kernel/cipher.h>

#define AES_BLOCK_SIZE 16

typedef struct {
    uint8_t         key[32];
    size_t          keylen;
    struct ci_hdr   hdr;
    struct dma_desc dd_hdr;
    struct dma_desc dd_key;
    struct dma_desc dd_iv;
    struct dma_desc dd_input;
    struct dma_desc dd_output;
} AES_CTX;

void aes_init(AES_CTX *ctx);
int aes_set_key(AES_CTX *ctx, const void *key, size_t len);
int aes_encrypt(AES_CTX *ctx, const void *input, size_t len, void *output);
int aes_decrypt(AES_CTX *ctx, const void *input, size_t len, void *output);
int aes_encrypt_cbc(AES_CTX *ctx, void *iv, const void *input, size_t len, void *output);
int aes_decrypt_cbc(AES_CTX *ctx, void *iv, const void *input, size_t len, void *output);
int aes_cmac(AES_CTX *ctx, void *iv, const void *input, size_t len);
