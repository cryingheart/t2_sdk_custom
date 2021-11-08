#pragma once
#include <kernel/assert.h>
#include <kernel/cdefs.h>
#include <kernel/compiler.h>

#include <bits/static_assert.h>

#ifdef NDEBUG
#define __assert_file_line(_message)
#else
#define __assert_file_line(_message)\
    __assert_fail("(" __FILE__ ":" stringify(__LINE__) ") " _message)
#endif


/* Assert that expr is true, also "return" value of expr. If compiled
 * with NDEBUG reduces to "expr". */
#define assert_true(expr) ({               \
            bool expr_ = (expr);           \
            if(!expr_)                     \
                __assert_file_line(#expr); \
            expr_;                         \
        })

/* Assert that expr is false, also "return" value of expr. If compiled
 * with NDEBUG reduces to "expr". */
#define assert_false(expr) ({                       \
            bool expr_ = (expr);                    \
            if(expr_)                               \
                __assert_file_line("!(" #expr ")"); \
            expr_;                                  \
        })

#define assert(expr) do { assert_true(expr); } while(0)
