#pragma once

#include <bits/static_assert.h>

/* this files contains function-like macros that doesn't correspond to
 * any specific module */

#define ALIGN_MASK(_x, _mask)   (((_x) + (_mask)) & ~(_mask))
#define ALIGN(_x, _balign)      ALIGN_MASK(_x, (typeof(_x))(_balign) - 1)
#define ALIGN_PTR(_ptr, _align) ((typeof(_ptr))ALIGN((uintptr_t)(_ptr), (_align)))

/* Given _len, return number N such that _len + N mod _pad = 0.
 *
 *  _pad must be power of two.
 */
#define PADDING(_len, _pad) (-(_len) & ((_pad)-1))

#define howmany(x, y)   (((x)+((y)-1))/(y))

#define assert_aligned(_ptr, _align) \
    assert(ALIGN_PTR((_ptr), (_align)) == (_ptr))

#define ARRAY_SIZE(_a) (sizeof(_a)/sizeof((_a)[0]))

/* true iff _PTRLIKE is known to be an array */
#define ARRAY_P(_ptrlike)                                        \
    __builtin_types_compatible_p(__typeof__((_ptrlike)[0])[],    \
                                 __typeof__(_ptrlike))

#define ARRAY_FOREACH(_p, _a, ...)                                \
    for(__VA_ARGS__ (_p) = &((_a)[0]);                           \
        (_p) < &((_a)[ARRAY_SIZE(_a)]);                           \
        (_p)++)

#define __stringify(_s) #_s
#define stringify(_s) __stringify(_s)

#ifdef __ASSEMBLER__
#define BIT(_n)     (1 << (_n))
#else
#define BIT(_n)     (1u << (_n))
#define BIT64(_n)   (UINT64_C(1) << (_n))
#endif
#define BITMASK(_w) (BIT(_w) - 1)
#define BITMASK64(_w) (BIT64(_w) - 1)
#define BITMASK2(_h, _l)  (BIT((_h)+1)-BIT(_l))

/* loop over all one bits in _value in ascending order, does not
 * modify _value */
#define FOREACH_BIT_SET(_bit, _value)                    \
    for(unsigned int __value = (_value), __bit = 0;      \
        __value != 0 &&                                  \
            (__bit = __builtin_ctz(__value)) >= 0 &&     \
            (__value &= ~(1u << __bit)) >= 0 &&          \
            ((_bit) = __bit) >= 0;                       \
        __bit++)

/* check that the types of x and y are compatible, if either are a
 * known constant they are assumed to be compatible; xs and ys are
 * string representations of x and y */
#define assert_compatible(x, xs, y, ys)                                 \
    static_assert(                                                      \
      __builtin_constant_p(x)                                           \
      || __builtin_constant_p(y)                                        \
      || __builtin_types_compatible_p(typeof(x),                        \
                                      typeof(y)),                       \
      xs " and " ys " are incompatible")

#define min(x, y) \
    ({                                  \
     assert_compatible(x, #x, y, #y);   \
     typeof(x) _min1 = (x);             \
     typeof(y) _min2 = (y);             \
     _min1 < _min2 ? _min1 : _min2;     \
     })

#define max(x, y) \
    ({                                  \
     assert_compatible(x, #x, y, #y);   \
     typeof(x) _max1 = (x);             \
     typeof(y) _max2 = (y);             \
     _max1 > _max2 ? _max1 : _max2;     \
     })

#define clamp(x, lo, hi)        min(hi,max(x,lo))

#define swap(x,y) \
    do {                        \
        typeof(x) _tmp = x;     \
        x = y;                  \
        y = _tmp;               \
    } while(0)

/* returns 2**k <= value if value is in range [1,2**32-1] else zero */
#define ROUND_DOWN_P2(_value) ({                                        \
            uint32_t __v = ~0u >> __builtin_clz(_value);                \
            __v - (__v >> 1);                                           \
        })

/* returns 2**k >= value if value is in range [1,2**31] else zero */
#define ROUND_UP_P2(_value) ({                                          \
            uint32_t __v = ~0u >> __builtin_clz((_value)-1);            \
            __v + 1;                                                    \
        })

/**
 * Cast a member of a structure out to the containing structure
 *
 * @param ptr The pointer to the member.
 * @param type The type of the container struct this is embedded in.
 * @param member The name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})
