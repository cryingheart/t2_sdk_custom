#pragma once

#if __linux__
#include "mbedtls/sha256.h"
#include "mbedtls/md.h"


#define SHA256_DIGEST_LENGTH 32
#define SHA256_CTX mbedtls_sha256_context

#include <dragonfly/df-cpp-begin.h>

void sha256_init(SHA256_CTX *ctx);

void
sha256_update(SHA256_CTX *ctx, const void *buf, size_t len);

void
sha256_final(SHA256_CTX *ctx, uint8_t digest[SHA256_DIGEST_LENGTH]);


#define HMAC_CTX mbedtls_md_context_t

enum hmac_alg {
    HMAC_ALG_MD5,
    HMAC_ALG_SHA1,
    HMAC_ALG_SHA256
};

void hmac_init(HMAC_CTX *ctx, enum hmac_alg alg,
               const void *key, size_t key_len);

void hmac_update(HMAC_CTX *ctx, const void *data, size_t len);

void hmac_final(HMAC_CTX *ctx, void *result, size_t len);

#include <dragonfly/df-cpp-end.h>

#else
#include "hmac.h"
#include "sha2.h"
#endif
