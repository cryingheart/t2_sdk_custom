#pragma once

#include <stdint.h>

#include <bits/null.h>
#include <bits/size_t.h>
#include <bits/time_t.h>
#include <bits/clockid_t.h>
#include <bits/restrict.h>

#define CLOCKS_PER_SEC 1000000

#define TIME_UTC 1

typedef uint64_t clock_t;

struct timespec {
    time_t tv_sec;
    long   tv_nsec;
};

struct tm {
    int         tm_year;
    int         tm_mon;
    int         tm_mday;
    int         tm_hour;
    int         tm_min;
    int         tm_sec;
    int         tm_wday;
    int         tm_yday;
    int         tm_isdst;
    /**/
    int         tm_gmtoff;
    const char *tm_zone;
};

#define __not_implemented(_name) \
    __attribute__((__warning__(#_name " is not implemented"))) _name

/* BEGIN GENERATED PROTOS */

char* asctime(const struct tm* timeptr);

clock_t clock(void);

int clock_getres(clockid_t clock_id, struct timespec* res);

int clock_gettime(clockid_t clock_id, struct timespec* tp);

int clock_settime(clockid_t clock_id, const struct timespec* tp);

char* ctime(const time_t* timer);

struct tm* gmtime(const time_t* t);

struct tm* gmtime_r(const time_t* restrict t, struct tm* restrict tm);

struct tm* localtime(const time_t* t);

struct tm*
localtime_r(const time_t* restrict t,
            struct tm* restrict tm);

time_t mktime(struct tm* tm);

size_t
strftime(char* restrict s,
         size_t n,
         const char* restrict f,
         const struct tm* restrict tm);

time_t time(time_t* timer);

int timespec_get(struct timespec* ts, int base);

/* END GENERATED PROTOS */

static inline double difftime(time_t time1, time_t time0)
{
    return time1-time0;
}
