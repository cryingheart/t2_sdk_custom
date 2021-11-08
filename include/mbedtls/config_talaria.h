#include <kernel/os.h>
#include <stdio.h>
#include <string.h>

#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_PLATFORM_NO_STD_FUNCTIONS

#define MBEDTLS_PLATFORM_EXIT_ALT
//#define MBEDTLS_PLATFORM_TIME_MACRO      time
#define MBEDTLS_PLATFORM_PRINTF_MACRO    os_printf
#define MBEDTLS_PLATFORM_SNPRINTF_MACRO  snprintf
#define MBEDTLS_PLATFORM_CALLOC_MACRO    os_calloc
#define MBEDTLS_PLATFORM_FREE_MACRO      os_free

#undef MBEDTLS_HAVE_TIME_DATE
#undef MBEDTLS_TIMING_C
#undef MBEDTLS_NET_C

#define MBEDTLS_AES_ALT
#define MBEDTLS_MD5_ALT
#define MBEDTLS_SHA1_ALT
#define MBEDTLS_SHA256_ALT

#define MBEDTLS_NO_PLATFORM_ENTROPY
#define MBEDTLS_ENTROPY_HARDWARE_ALT

#undef MBEDTLS_SELF_TEST
#undef MBEDTLS_DEBUG_C

#define MBEDTLS_ENTROPY_FORCE_SHA256
#define MBEDTLS_CTR_DRBG_ENTROPY_LEN            32 /**< Amount of entropy used per seed by default (48 with SHA-512, 32 with SHA-256) */

#define MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES    5 /**< Maximum entries in cache */

#define MBEDTLS_SSL_MAX_CONTENT_LEN            16384

#define MBEDTLS_X509_MAX_FILE_PATH_LEN         128 /**< Maximum length of a path/filename string in bytes including the null terminator character ('\0'). */
