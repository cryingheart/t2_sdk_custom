#pragma once

#include <stdbool.h>
#include "mbedtls/bignum.h"
#include "mbedtls/ecp.h"

#include <dragonfly/df-cpp-begin.h>

typedef mbedtls_mpi df_mpint;

typedef mbedtls_ecp_point df_ec_point;

enum df_ec_group_id {
    DF_EC_GROUP_UNDEFINED = 0,
    DF_EC_GROUP_IKE_19 = 19,
    DF_EC_GROUP_IKE_20 = 20,
    DF_EC_GROUP_IKE_21 = 21
};

typedef struct df_ec_group  {
    enum df_ec_group_id id;
    mbedtls_ecp_group group;
} df_ec_group;

#define DF_MPINT_INITIALIZER { .s = 1, .n = 0, .p = NULL }

#define DECLARE_MPINT(name_)                                             \
    df_mpint name_ __cleanup(df_mpi_free) = DF_MPINT_INITIALIZER

#define DECLARE_POINT(name_)                                            \
    df_ec_point name_ __cleanup(df_ec_point_free) = {                   \
        .X = DF_MPINT_INITIALIZER,                                      \
        .Y = DF_MPINT_INITIALIZER,                                      \
        .Z = DF_MPINT_INITIALIZER                                       \
    }

/************************************************************/

/************************************************************/

/* BEGIN GENERATED PROTOS */

void df_mpi_init(df_mpint* z);

size_t df_mpi_size(const df_mpint* x);

enum dragonfly_status df_mpi_init_int(df_mpint* z, int value);

enum dragonfly_status df_mpi_copy(df_mpint* z, const df_mpint* x);

void df_mpi_free(df_mpint* z);

enum dragonfly_status
df_mpi_add(df_mpint* z,
           const df_mpint* x,
           const df_mpint* y);

enum dragonfly_status
df_mpi_add_int(df_mpint* z,
               const df_mpint* x,
               int y);

enum dragonfly_status
df_mpi_sub(df_mpint* z,
           const df_mpint* x,
           const df_mpint* y);

enum dragonfly_status
df_mpi_sub_int(df_mpint* z,
               const df_mpint* x,
               int y);

enum dragonfly_status
df_mpi_mod(df_mpint* z,
           const df_mpint* x,
           const df_mpint* m);

enum dragonfly_status
df_mpi_mul_mod(df_mpint* z,
               const df_mpint* x,
               const df_mpint* y,
               const df_mpint* m);

enum dragonfly_status
df_mpi_mul(df_mpint* z,
           const df_mpint* x,
           const df_mpint* y);

enum dragonfly_status
df_mpi_mul_int(df_mpint* z,
               const df_mpint* x,
               unsigned int y);

int df_mpi_cmp(const df_mpint* x, const df_mpint* y);

int df_mpi_cmp_int(const df_mpint* x, int y);

enum dragonfly_status
df_mpi_exp_mod(df_mpint* z,
               const df_mpint* x,
               const df_mpint* y,
               const df_mpint* m);

enum dragonfly_status
df_mpi_shr(df_mpint* z,
           const df_mpint* x,
           unsigned int n);

enum dragonfly_status
df_mpi_invert(df_mpint* z,
              const df_mpint* x,
              const df_mpint* m);

enum dragonfly_status
df_mpi_to_binary(const df_mpint* x,
                 void* dst,
                 size_t size);

enum dragonfly_status
df_mpi_from_binary(df_mpint* z,
                   const void* src,
                   size_t size);

int
df_mpi_to_string(char* buf,
                 size_t size,
                 const df_mpint* value,
                 unsigned int radix);

int df_mpi_lsb(const df_mpint* x);

enum dragonfly_status
df_mpi_random(df_mpint* rndint,
              const df_mpint* order);

void df_ec_point_init(df_ec_point* x);

enum dragonfly_status
df_ec_point_copy(df_ec_point* z,
                 const df_ec_point* x);

void df_ec_point_free(df_ec_point* x);

enum dragonfly_status
df_ec_point_set(df_ec_point* p,
                const df_mpint* x,
                const df_mpint* y);

bool df_ec_point_is_infinity(const df_ec_point* x);

enum dragonfly_status df_ec_point_set_infinity(df_ec_point* z);

enum dragonfly_status
df_ec_point_cmp(const df_ec_point* x,
                const df_ec_point* y);

void df_ec_group_init(df_ec_group* group);

void df_ec_group_free(df_ec_group* group);

enum dragonfly_status
df_ec_group_load(df_ec_group* group,
                 enum df_ec_group_id group_id);

enum dragonfly_status
df_ec_group_eval(const df_ec_group* group,
                 df_mpint* y2,
                 const df_mpint* x);

enum dragonfly_status
df_ec_group_add(const df_ec_group* group,
                df_ec_point* Z,
                const df_ec_point* X,
                const df_ec_point* Y);

enum dragonfly_status
df_ec_group_mul(const df_ec_group* group,
                df_ec_point* Z,
                const df_mpint* x,
                const df_ec_point* Y);

enum dragonfly_status
df_ec_group_invert(const df_ec_group* group,
                   df_ec_point* Z,
                   const df_ec_point* X);

/* END GENERATED PROTOS */

static inline const df_mpint*
df_ec_group_prime(const df_ec_group *g)
{
    return &g->group.P;
}

/* return bit-length of prime */
static inline size_t
df_ec_group_prime_length(const df_ec_group *g)
{
    return g->group.pbits;
}

static inline const df_mpint*
df_ec_group_order(const df_ec_group *g)
{
    return &g->group.N;
}

#include <dragonfly/df-cpp-end.h>
