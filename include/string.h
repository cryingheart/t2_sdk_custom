#pragma once

#include <bits/size_t.h>
#include <bits/null.h>
#include <bits/restrict.h>

/* BEGIN GENERATED PROTOS */

void* memchr(const void* src, int c, size_t n);

int memcmp(const void* vl, const void* vr, size_t n);

void* memcpy(void* restrict dest, const void* restrict src, size_t n);

void* memmove(void* dest, const void* src, size_t n);

void* memrchr(const void* m, int c, size_t n);

void* memset(void* dest, int c, size_t n);

void memswp(void* base, size_t i, size_t j, size_t size);

int strcasecmp(const char* s1, const char* s2);

char* strcat(char* restrict dest, const char* restrict src);

char* strchr(const char* s, int c);

char* strchrnul(const char* s, int c);

int strcmp(const char* l, const char* r);

int strcoll(const char* s1, const char* s2);

char* strcpy(char* restrict dest, const char* restrict src);

size_t strcspn(const char* s, const char* c);

char* strdup(const char* s);

char* strerror(int errnum);

size_t strlcpy(char* dst, const char* src, size_t size);

size_t strlen(const char* s);

int strncasecmp(const char* s1, const char* s2, size_t n);

char* strncat(char* restrict d, const char* restrict s, size_t n);

int strncmp(const char* _l, const char* _r, size_t n);

char* strncpy(char* restrict d, const char* restrict s, size_t n);

char* strndup(const char* s, size_t size);

size_t strnlen_s(const char* s, size_t maxsize);

char* strpbrk(const char* s, const char* b);

char* strrchr(const char* s, int c);

char* strsep(char* *str, const char* delim);

size_t strspn(const char* s, const char* c);

char* strstr(const char* h, const char* n);

char* strtok(char* restrict s, const char* restrict sep);

char*
strtok_r(char* restrict s,
         const char* restrict sep,
         char* * restrict p);

size_t strxfrm(char* restrict s1, const char* restrict s2, size_t n);

/* END GENERATED PROTOS */

#define strnlen strnlen_s
