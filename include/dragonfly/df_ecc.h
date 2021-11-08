#pragma once

#include <dragonfly/df_mpint.h>

#include <dragonfly/df-cpp-begin.h>

/* BEGIN GENERATED PROTOS */

enum dragonfly_status
df_ecc_elem_op(const df_ec_group* group,
               df_ec_point* Z,
               const df_ec_point* X,
               const df_ec_point* Y);

enum dragonfly_status
df_ecc_scalar_op(const df_ec_group* group,
                 df_ec_point* Z,
                 const df_mpint* x,
                 const df_ec_point* Y);

enum dragonfly_status
df_ecc_inverse_op(const df_ec_group* group,
                  df_ec_point* Z,
                  const df_ec_point* X);

enum dragonfly_status
df_ecc_compute_shared_secret(const df_ec_group* group,
                             const df_ec_point* pwe,
                             const df_mpint* rand,
                             const df_mpint* scalar,
                             const df_ec_point* element,
                             df_mpint* secret);

/* END GENERATED PROTOS */

#include <dragonfly/df-cpp-end.h>
