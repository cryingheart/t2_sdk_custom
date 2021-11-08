#pragma once

#include <assert.h>
#include <bits/size_t.h>
#include <bits/ssize_t.h>
#include <bits/null.h>
#include <bits/seek.h>
#include <bits/file.h>
#include <bits/restrict.h>

#define _IOFBF 1
#define _IOLBF 2
#define _IONBF 3

#define BUFSIZ 248

#define EOF -1

#define FOPEN_MAX 8

#define FILENAME_MAX 17

#define L_tmpnam 17

#define TMP_MAX 1

typedef unsigned int fpos_t;

extern FILE *stderr;
extern FILE *stdin;
extern FILE *stdout;

int putc(int c, FILE* stream);

int getc(FILE* stream);

/* BEGIN GENERATED PROTOS */

void clearerr(FILE* stream);

int fclose(FILE* stream);

int feof(FILE* stream);

int ferror(FILE* stream);

int fflush(FILE* stream);

int fgetc(FILE* stream);

int fgetpos(FILE* restrict stream, fpos_t* restrict pos);

char* fgets(char* restrict s, int n, FILE* restrict stream);

FILE*
fmemopen(void* restrict buf,
         size_t size,
         const char* restrict mode);

FILE* fopen(const char* restrict filename, const char* restrict mode);

int fprintf(FILE* restrict stream, const char* restrict format, ...)
    __attribute__((__format__(__printf__, 2, 3)));

int fputc(int c, FILE* stream);

int fputs(const char* restrict s, FILE* restrict stream);

size_t
fread(void* restrict ptr,
      size_t size,
      size_t nmemb,
      FILE* restrict stream);

FILE*
freopen(const char* restrict filename,
        const char* restrict mode,
        FILE* restrict stream);

FILE* fropen(void* cookie, int (*readfn)(void*, char*, int));

int fscanf(FILE* restrict stream, const char* restrict format, ...)
    __attribute__((__format__(__scanf__, 2, 3)));

int fseek(FILE* stream, long int offset, int whence);

int fsetpos(FILE* stream, const fpos_t* pos);

long int ftell(FILE* stream);

FILE*
funopen(const void* cookie,
        int (*readfn)(void*, char*, int),
        int (*writefn)(void*, const char*, int),
        off_t (*seekfn)(void*, off_t, int),
        int (*closefn)(void*));

FILE* fwopen(void* cookie, int (*writefn)(void*, const char*, int));

size_t
fwrite(const void* restrict ptr,
       size_t size,
       size_t nmemb,
       FILE* restrict stream);

int getc(FILE* stream);

int getchar(void);

void perror(const char* s);

int printf(const char* restrict format, ...)
    __attribute__((__format__(__printf__, 1, 2)));

int putc(int c, FILE* stream);

int putchar(int c);

int puts(const char* s);

int remove(const char* filename);

int rename(const char* old, const char* new);

void rewind(FILE* stream);

int scanf(const char* restrict format, ...)
    __attribute__((__format__(__scanf__, 1, 2)));

void setbuf(FILE* restrict stream, char* restrict buf);

int
setvbuf(FILE* restrict stream,
        char* restrict buf,
        int mode,
        size_t size);

int
snprintf(char* restrict s,
         size_t n,
         const char* restrict format,
         ...)
    __attribute__((__format__(__printf__, 3, 4)));

int sscanf(const char* restrict s, const char* restrict format, ...)
    __attribute__((__format__(__scanf__, 2, 3)));

FILE* tmpfile(void);

char* tmpnam(char* s);

int ungetc(int c, FILE* stream);

int
vfprintf(FILE* restrict stream,
         const char* restrict format,
         va_list arg)
    __attribute__((__format__(__printf__, 2, 0)));

int
vfscanf(FILE* restrict stream,
        const char* restrict format,
        va_list arg)
    __attribute__((__format__(__scanf__, 2, 0)));

int vprintf(const char* restrict format, va_list arg)
    __attribute__((__format__(__printf__, 1, 0)));

int vscanf(const char* restrict format, va_list arg)
    __attribute__((__format__(__scanf__, 1, 0)));

int
vsnprintf(char* restrict s,
          size_t n,
          const char* restrict format,
          va_list arg)
    __attribute__((__format__(__printf__, 3, 0)));

int
vsscanf(const char* restrict s,
        const char* restrict format,
        va_list arg)
    __attribute__((__format__(__scanf__, 2, 0)));

/* END GENERATED PROTOS */

#define sprintf(_s, ...) ({                                             \
            static_assert(ARRAY_P(_s),                                  \
                          "please pass only array as first argument of sprintf"); \
            snprintf((_s), sizeof(_s), __VA_ARGS__);                    \
        })

#define vsprintf(_s, _ap) ({                                            \
            static_assert(ARRAY_P(_s),                                  \
                          "please pass only array as first argument of vsprintf"); \
            vsnprintf((_s), sizeof(_s), _ap);                           \
        })

#define gets(_s) ({                                                     \
            static_assert(ARRAY_P(_s),                                  \
                          "please pass only array to gets");            \
            fgets((_s), sizeof(_s), stdin);                             \
        })
