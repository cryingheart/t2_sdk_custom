#pragma once
#include <bits/time_t.h>
#include <bits/suseconds_t.h>
#include <bits/restrict.h>

struct timeval {
    time_t tv_sec;
    suseconds_t tv_usec;
};

/* BEGIN GENERATED PROTOS */

int gettimeofday(struct timeval* restrict tp, void* restrict tzp);

uint64_t timeval_to_usec(const struct timeval* timeval);

struct timeval usec_to_timeval(uint64_t timestamp);

/* END GENERATED PROTOS */
