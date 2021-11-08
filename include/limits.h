#pragma once

#define CHAR_BIT __CHAR_BIT__

#define SCHAR_MIN (-1-__SCHAR_MAX__)
#define SCHAR_MAX __SCHAR_MAX__

#define UCHAR_MAX 0xff

#if __CHAR_UNSIGNED__
#define CHAR_MIN 0
#define CHAR_MAX UCHAR_MAX
#else
#define CHAR_MIN SCHAR_MIN
#define CHAR_MAX SCHAR_MAX
#endif

#define MB_LEN_MAX 1

#define SHRT_MIN (-1-__SHRT_MAX__)
#define SHRT_MAX __SHRT_MAX__

#define USHRT_MAX 0xffff

#define INT_MIN (-1-__INT_MAX__)
#define INT_MAX __INT_MAX__

#define UINT_MAX 0xffffffffU

#define LONG_MIN (-1-__LONG_MAX__)
#define LONG_MAX __LONG_MAX__

#define ULONG_MAX 0xffffffffUL

#define LLONG_MIN -0x8000000000000000LL
#define LLONG_MAX  0x7fffffffffffffffLL

#define ULLONG_MAX 0xffffffffffffffffULL
