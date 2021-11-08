#pragma once
#include <stddef.h>
#include <stdlib.h>

struct testcase {
    const char *name;
    int (*fn)(void);
    unsigned int flags;
};

#define TEST_DISABLE_CHIP    BIT(0) /* chip, physical or simulated */
#define TEST_DISABLE_CHIP_HW BIT(1) /* chip, physical */
#define TEST_DISABLE_FPGA    BIT(2) /* fpga, physical or simulated */
#define TEST_DISABLE_NSIM    BIT(3)
#define TEST_DISABLE_SLOW    BIT(4) /* any simulation */

#define TEST_NO_HEAP_INVARIANT BIT(8) /* test does not preserve
                                       * available heap */

#define verify_eq(_exp, _val) \
    do { \
        if ((_exp) != (_val)) {\
            os_printf("VERIFY FAIL: %s = %lld, expected == %s\n", #_exp, (int64_t)(_exp), #_val); \
            return __LINE__; \
        } \
    } while (0)

#define verify_ne(_exp, _val) \
    do { \
        if ((_exp) == (_val)) {\
            os_printf("VERIFY FAIL: %s = %d, expected != %s\n", #_exp, _exp, #_val); \
            return __LINE__; \
        } \
    } while (0)

#define verify_diff(_a, _b, _diff) \
    do { \
        if (abs((_a) - (_b)) > (_diff)) {\
            os_printf("VERIFY FAIL: |%d - %d| <= %d\n", _a, _b, _diff); \
            return __LINE__; \
        } \
    } while (0)

#define verify(_exp) \
    do { \
        if (!(_exp)) {\
            os_printf("VERIFY FAIL: %s\n", #_exp);      \
            return __LINE__; \
        } \
    } while (0)

#define verify_ret(_exp, _err) \
    do { \
        if (!(_exp)) { \
            os_printf("VERIFY FAIL: %s\n", #_exp);     \
            return _err; \
        } \
    } while (0)

/**
 * Wait a maximum amount of cycles for some SR register get a value matching
 * the mask/value .
 */
#define wait_for(_exp, _tmo) \
    ({ \
        int i; \
        int e; \
        for (i = _tmo; i > 0 && !(e=(_exp)); --i) \
            /*nothing */; \
        (i > 0 ? 0 : __LINE__); \
    })

/**
 * Wait a maximum amount of cycles for some MMIO register get a value matching
 * the mask/value .
 */
#define __wait_rd(_tmo, _op, _addr, _mask, _value) \
    ({ \
        int i; \
        for (i = _tmo; i > 0 && ((_op(_addr) & _mask) != _value) ; --i) \
            /*nothing */; \
        (i > 0 ? 0 : __LINE__); \
    })

#define wait_rd32(_tmo, _addr, _mask, _value) \
    __wait_rd(_tmo, mmio_rd32, _addr, _mask, _value)

#define wait_rd16(_tmo, _addr, _mask, _value) \
    __wait_rd(_tmo, mmio_rd16, _addr, _mask, _value)

#define wait_rd8(_tmo, _addr, _mask, _value) \
    __wait_rd(_tmo, mmio_rd8, _addr, _mask, _value)

int  hwtest_run(const char *suite, const struct testcase *tc, size_t n);
