/**
 * SHA-2
 */
#pragma once
#include <stdint.h>
#include <kernel/cipher.h>

#define SHA224_DIGEST_LENGTH		28

#define SHA256_BLOCK_LENGTH		64
#define SHA256_DIGEST_LENGTH		32
#define SHA256_DIGEST_STRING_LENGTH	(SHA256_DIGEST_LENGTH * 2 + 1)

typedef struct {
	uint32_t	state[8];
	size_t          count;
	uint8_t	        buffer[SHA256_BLOCK_LENGTH];
} SHA256_CTX;

#define SHA512_BLOCK_LENGTH		128
#define SHA512_DIGEST_LENGTH		64
#define SHA512_DIGEST_STRING_LENGTH	(SHA512_DIGEST_LENGTH * 2 + 1)

typedef struct {
	uint64_t	state[8];
	uint32_t	count;
	uint8_t	        buffer[SHA512_BLOCK_LENGTH];
} SHA512_CTX;

void sha256_init(SHA256_CTX *ctx);
void sha256_update(SHA256_CTX *ctx, const void *buf, size_t len);
void sha256_final(SHA256_CTX *ctx, uint8_t *digest);

void sha224_init(SHA256_CTX *ctx);
void sha224_update(SHA256_CTX *ctx, const void *buf, size_t len);
void sha224_final(SHA256_CTX *ctx, uint8_t *digest);

void sha256_sw_init(SHA256_CTX *ctx);
void sha256_sw_update(SHA256_CTX *ctx, const void *buf, size_t len);
void sha256_sw_final(SHA256_CTX *ctx, uint8_t *digest);

void sha512_sw_init(SHA512_CTX *ctx);
void sha512_sw_update(SHA512_CTX *ctx, const void *buf, size_t len);
void sha512_sw_final(SHA512_CTX *ctx, uint8_t *digest);
