#pragma once
#include <stdint.h>
#include <kernel/dma.h>
#include <kernel/cipher.h>

typedef struct {
    uint8_t         key[16];
    struct ci_hdr   hdr;
    struct dma_desc dd_hdr;
    struct dma_desc dd_key;
    struct dma_desc dd_input;
    struct dma_desc dd_output;
} RC4_CTX;

void rc4_init(RC4_CTX *ctx);
int rc4_set_key(RC4_CTX *ctx, const void *key, size_t len);
int rc4_encrypt(RC4_CTX *ctx, const void *input, size_t len, void *output);
int rc4_decrypt(RC4_CTX *ctx, const void *input, size_t len, void *output);
