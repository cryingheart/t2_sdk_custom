#pragma once
#include <stddef.h>
#include <stdint.h>
#include <kernel/cipher.h>

#define SHA1_BLOCK_LENGTH  64
#define SHA1_DIGEST_LENGTH 20U

typedef struct {
    uint32_t      count;
    uint16_t      pad;
    struct ci_hdr hdr;
    uint32_t      state[5];
    unsigned char buffer[SHA1_BLOCK_LENGTH];
} SHA1_CTX;

void sha1_init(SHA1_CTX *context);
void sha1_update(SHA1_CTX *context, const void *data, size_t len);
void sha1_final(SHA1_CTX *context, uint8_t digest[SHA1_DIGEST_LENGTH]);
void _sha1_output(SHA1_CTX *context, uint8_t digest[SHA1_DIGEST_LENGTH]);

void sha1_sw_init(SHA1_CTX *context);
void sha1_sw_update(SHA1_CTX *context, const void *data, size_t len);
void sha1_sw_final(SHA1_CTX *context, uint8_t digest[SHA1_DIGEST_LENGTH]);
void _sha1_sw_output(SHA1_CTX *context, uint8_t digest[SHA1_DIGEST_LENGTH]);

void pbkdf2_sha1(const char *passwd, const char *salt, size_t iterations, uint8_t *key, size_t keylen);
