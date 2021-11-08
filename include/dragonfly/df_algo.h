#pragma once

#include <dragonfly/df_mpint.h>

#include <dragonfly/df-cpp-begin.h>

/* BEGIN GENERATED PROTOS */

enum dragonfly_status
df_legendre_symbol(df_mpint* r,
                   const df_mpint* a,
                   const df_mpint* p);

enum dragonfly_status
df_check_quadratic_residue(const df_ec_group* group,
                           const df_mpint* y2);

enum dragonfly_status
df_sqrt(df_mpint* n,
        const df_mpint* n2,
        const df_mpint* p);

enum dragonfly_status
df_construct_pe(df_ec_point* pwe,
                const df_mpint* prime,
                const df_mpint* x,
                const df_mpint* y,
                int mod2_selector);

/* END GENERATED PROTOS */

#include <dragonfly/df-cpp-end.h>
