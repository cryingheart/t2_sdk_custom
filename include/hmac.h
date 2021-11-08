#pragma once

#include <stdbool.h>
#include <stddef.h>

#include <md5.h>
#include <sha1.h>
#include <sha2.h>

enum hmac_alg {
    HMAC_ALG_MD5,
    HMAC_ALG_SHA1,
    HMAC_ALG_SHA256
};

#define HMAC_MAX_DIGEST_LENGTH SHA256_DIGEST_LENGTH

struct hash_method;

struct hmac {
    unsigned char B[64];
    const struct hash_method *hash;
    union {
        MD5_CTX md5;
        SHA1_CTX sha1;
        SHA256_CTX sha256;
    } context;
};
typedef struct hmac HMAC_CTX;

void hmac_init(HMAC_CTX *ctx, enum hmac_alg alg,
               const void *key, size_t key_len);
size_t hmac_digest_length(HMAC_CTX *ctx);
void hmac_update(HMAC_CTX *ctx, const void *data, size_t len);
void hmac_final(HMAC_CTX *ctx, void *result, size_t len);

void md5_hmac(const void *msg, size_t len,
              const unsigned char *key, size_t keylen,
              void *digest);

void sha1_hmac(const void *msg, size_t len,
               const uint8_t *key, size_t keylen,
               void *digest);
