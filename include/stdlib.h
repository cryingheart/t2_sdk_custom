#pragma once

#include <bits/null.h>

#include <bits/size_t.h>
#include <bits/wchar_t.h>
#include <bits/noreturn.h>
#include <bits/restrict.h>

typedef struct  {
    int quot;
    int rem;
} div_t;

typedef struct  {
    long quot;
    long rem;
} ldiv_t;

typedef struct  {
    long long quot;
    long long rem;
} lldiv_t;

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define RAND_MAX 0x7fffffff

#define MB_CUR_MAX 1

#define malloc  os_alloc
#define free    os_free
#define realloc os_realloc

/* non-standard function equivalent to:
 *   v = *value;
 *   *value /= divisor;
 *   return v % divisor
 * 1 <= divisor < 2**16
 */
unsigned short udiv_64_16(unsigned long long *value, unsigned short divisor);

/* non-standard integer square-root */
unsigned int sqrti(unsigned int x);

/* BEGIN GENERATED PROTOS */

void abort(void) __noreturn;

int abs(int j);

int atoi(const char* nptr);

long atol(const char* nptr);

long long atoll(const char* nptr);

void*
bsearch(const void* key,
        const void* base,
        size_t nel,
        size_t width,
        int (*cmp)(const void*, const void*));

div_t div(int num, int den);

long labs(long j);

ldiv_t ldiv(long num, long den);

long long llabs(long long j);

lldiv_t lldiv(long long num, long long den);

void
qsort(void* base,
      size_t nmemb,
      size_t size,
      int (*compar)(const void*, const void*));

int rand(void);

void srand(unsigned int seed);

long
strtol(const char* restrict nptr,
       char* * restrict endptr,
       int base);

long long
strtoll(const char* restrict nptr,
        char* * restrict endptr,
        int base);

unsigned long
strtoul(const char* restrict nptr,
        char* * restrict endptr,
        int base);

unsigned long long
strtoull(const char* restrict nptr,
         char* * restrict endptr,
         int base);

/* END GENERATED PROTOS */

int
parse_macaddr(const char *string, unsigned char result[])
    __attribute__((__nonnull__(1, 2)));

size_t
parse_hexstring(const char *string, char **endptr, void *output, size_t size)
     __attribute__((__nonnull__(1)));

int
pct_decode(void *output, size_t output_size,
           const char *input, size_t input_length);

int
pct_encode(char *output, size_t output_size,
           const void *input, size_t input_length);

#define abs(j)   __builtin_abs(j)
#define labs(j)  __builtin_labs(j)
#define llabs(j) __builtin_llabs(j)

static inline char* getenv(const char *name)
{
    return NULL;
}

static inline int system(const char *string)
{
    return string != NULL;
}
