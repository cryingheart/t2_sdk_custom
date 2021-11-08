#pragma once
#include <stdint.h>

#define _LITTLE_ENDIAN 1234
#define _BIG_ENDIAN    4321

#if defined(__arm__)
#define _BYTE_ORDER _LITTLE_ENDIAN
#else
#error Unsupported architecutre
#endif

#define	bswap16(x)	__builtin_bswap16(x)
#define	bswap32(x)	__builtin_bswap32(x)
#define	bswap64(x)	__builtin_bswap64(x)

#if _BYTE_ORDER == _LITTLE_ENDIAN
#define	htobe16(x)	bswap16((x))
#define	htobe32(x)	bswap32((x))
#define	htobe64(x)	bswap64((x))
#define	htole16(x)	((uint16_t)(x))
#define	htole32(x)	((uint32_t)(x))
#define	htole64(x)	((uint64_t)(x))
#define	be16toh(x)	bswap16((x))
#define	be32toh(x)	bswap32((x))
#define	be64toh(x)	bswap64((x))
#define	le16toh(x)	((uint16_t)(x))
#define	le32toh(x)	((uint32_t)(x))
#define	le64toh(x)	((uint64_t)(x))
#elif _BYTE_ORDER == _BIG_ENDIAN
#define	htobe16(x)	((uint16_t)(x))
#define	htobe32(x)	((uint32_t)(x))
#define	htobe64(x)	((uint64_t)(x))
#define	htole16(x)	bswap16((x))
#define	htole32(x)	bswap32((x))
#define	htole64(x)	bswap64((x))
#define	be16toh(x)	((uint16_t)(x))
#define	be32toh(x)	((uint32_t)(x))
#define	be64toh(x)	((uint64_t)(x))
#define	le16toh(x)	bswap16((x))
#define	le32toh(x)	bswap32((x))
#define	le64toh(x)	bswap64((x))
#else
#error Bad _BYTE_ORDER
#endif

static inline uint16_t
be16_get(const void *_p)
{
#ifdef __ARM_FEATURE_UNALIGNED
    const uint16_t * const p = _p;
    return be16toh(*p);
#else
    const uint8_t * const p = _p;
    return ((p[0] << 8) | p[1]);
#endif
}

static inline uint32_t
be32_get(const void *_p)
{
#ifdef __ARM_FEATURE_UNALIGNED
    const uint32_t * const p = _p;
    return be32toh(*p);
#else
    uint8_t const * const p = _p;
    return ((uint32_t)p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
#endif
}

static inline uint16_t
le16_get(const void *_p)
{
#ifdef __ARM_FEATURE_UNALIGNED
    const uint16_t * const p = _p;
    return le16toh(*p);
#else
    const uint8_t * const p = _p;
    return p[0] | (p[1] << 8);
#endif
}

static inline uint32_t
le32_get(const void *_p)
{
#ifdef __ARM_FEATURE_UNALIGNED
    const uint32_t * const p = _p;
    return le32toh(*p);
#else
    const uint8_t * const p = _p;
    return (uint32_t)p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
#endif
}

static inline void *
be16_put(void *_p, uint16_t x)
{
#ifdef __ARM_FEATURE_UNALIGNED
    uint16_t * const p = _p;
    *p = htobe16(x);
    return p + 1;
#else
    uint8_t *p = _p;
    *p++ = (x >> 8) & 0xff;
    *p++ = (x >> 0) & 0xff;
    return p;
#endif
}

static inline void *
be32_put(void *_p, uint32_t x)
{
#ifdef __ARM_FEATURE_UNALIGNED
    uint32_t * const p = _p;
    *p = htobe32(x);
    return p + 1;
#else
    uint8_t *p = _p;
    *p++ = (x >> 24) & 0xff;
    *p++ = (x >> 16) & 0xff;
    *p++ = (x >>  8) & 0xff;
    *p++ = (x >>  0) & 0xff;
    return p;
#endif
}

static inline void *
le16_put(void *_p, uint16_t x)
{
#ifdef __ARM_FEATURE_UNALIGNED
    uint16_t * const p = _p;
    *p = htole16(x);
    return p + 1;
#else
    uint8_t *p = _p;
    *p++ = (x >> 0) & 0xff;
    *p++ = (x >> 8) & 0xff;
    return p;
#endif
}

static inline void *
le32_put(void *_p, uint32_t x)
{
#ifdef __ARM_FEATURE_UNALIGNED
    uint32_t * const p = _p;
    *p = htole32(x);
    return p + 1;
#else
    uint8_t *p = _p;
    *p++ = (x >>  0) & 0xff;
    *p++ = (x >>  8) & 0xff;
    *p++ = (x >> 16) & 0xff;
    *p++ = (x >> 24) & 0xff;
    return p;
#endif
}

#ifdef __ARM_FEATURE_UNALIGNED
static inline uint64_t
be64_get(const void *_p)
{
    const uint8_t * const p = _p;
    return ((uint64_t)be32_get(p) << 32) | be32_get(p + 4);
}

static inline uint64_t
le64_get(const void *p)
{
    uint32_t lo, hi;
    asm("ldr\t%0, [%1, #0]" :"=r"(lo) : "r"(p));
    asm("ldr\t%0, [%1, #4]" :"=r"(hi) : "r"(p));
    return (uint64_t)lo | ((uint64_t)hi << 32);
}

static inline void *
be64_put(void *_p, uint64_t x)
{
    uint8_t *p = _p;
    p = be32_put(p, x >> 32);
    p = be32_put(p, x & ~0u);
    return p;
}

static inline void *
le64_put(void *p, uint64_t x)
{
    uint32_t lo = x & ~0u;
    uint32_t hi = x >> 32;
    asm("str\t%0, [%1, #0]" ::"r"(lo), "r"(p));
    asm("str\t%0, [%1, #4]" ::"r"(hi), "r"(p));
    return p + 8;
}
#endif /* __ARM_FEATURE_UNALIGNED */
