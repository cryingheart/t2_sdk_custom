/* This is a generated file. */
#pragma once

/* BEGIN GENERATED PROTOS */

const char *mp_error_string(mp_result res);

mp_result
mp_int_abs(mp_int a,
           mp_int c);

mp_result
mp_int_add(mp_int a,
           mp_int b,
           mp_int c);

mp_result
mp_int_add_value(mp_int a,
                 mp_small value,
                 mp_int c);

mp_int
mp_int_alloc(void);

mp_result
mp_int_binary_len(mp_int z);

void
mp_int_clear(mp_int z);

int
mp_int_compare(mp_int a,
               mp_int b);

int
mp_int_compare_unsigned(mp_int a,
                        mp_int b);

int
mp_int_compare_uvalue(mp_int z,
                      mp_usmall uv);

int
mp_int_compare_value(mp_int z,
                     mp_small value);

int
mp_int_compare_zero(mp_int z);

mp_result
mp_int_copy(mp_int a,
            mp_int c);

mp_result
mp_int_count_bits(mp_int z);

mp_result
mp_int_div(mp_int a,
           mp_int b,
           mp_int q,
           mp_int r);

mp_result
mp_int_div_pow2(mp_int a,
                mp_small p2,
                mp_int q,
                mp_int r);

mp_result
mp_int_div_value(mp_int a,
                 mp_small value,
                 mp_int q,
                 mp_small *r);

int
mp_int_divisible_value(mp_int a,
                       mp_small v);

mp_result
mp_int_egcd(mp_int a,
            mp_int b,
            mp_int c,
            mp_int x,
            mp_int y);

mp_result
mp_int_expt(mp_int a,
            mp_small b,
            mp_int c);

mp_result
mp_int_expt_full(mp_int a,
                 mp_int b,
                 mp_int c);

mp_result
mp_int_expt_value(mp_small a,
                  mp_small b,
                  mp_int c);

mp_result
mp_int_exptmod(mp_int a,
               mp_int b,
               mp_int m,
               mp_int c);

mp_result
mp_int_exptmod_bvalue(mp_small value,
                      mp_int b,
                      mp_int m,
                      mp_int c);

mp_result
mp_int_exptmod_evalue(mp_int a,
                      mp_small value,
                      mp_int m,
                      mp_int c);

mp_result
mp_int_exptmod_known(mp_int a,
                     mp_int b,
                     mp_int m,
                     mp_int mu,
                     mp_int c);

void
mp_int_free(mp_int z);

mp_result
mp_int_gcd(mp_int a,
           mp_int b,
           mp_int c);

mp_result
mp_int_init(mp_int z);

mp_result
mp_int_init_copy(mp_int z,
                 mp_int old);

mp_result
mp_int_init_size(mp_int z,
                 mp_size prec);

mp_result
mp_int_init_uvalue(mp_int z,
                   mp_usmall uvalue);

mp_result
mp_int_init_value(mp_int z,
                  mp_small value);

mp_result
mp_int_invmod(mp_int a,
              mp_int m,
              mp_int c);

int
mp_int_is_pow2(mp_int z);

mp_result
mp_int_lcm(mp_int a,
           mp_int b,
           mp_int c);

mp_result
mp_int_mod(mp_int a,
           mp_int m,
           mp_int c);

mp_result
mp_int_mul(mp_int a,
           mp_int b,
           mp_int c);

mp_result
mp_int_mul_pow2(mp_int a,
                mp_small p2,
                mp_int c);

mp_result
mp_int_mul_value(mp_int a,
                 mp_small value,
                 mp_int c);

mp_result
mp_int_neg(mp_int a,
           mp_int c);

mp_result
mp_int_read_binary(mp_int z,
                   unsigned char *buf,
                   int len);

mp_result
mp_int_read_cstring(mp_int z,
                    mp_size radix,
                    const char *str,
                    char **end);

mp_result
mp_int_read_string(mp_int z,
                   mp_size radix,
                   const char *str);

mp_result
mp_int_read_unsigned(mp_int z,
                     unsigned char *buf,
                     int len);

mp_result
mp_int_redux_const(mp_int m,
                   mp_int c);

mp_result
mp_int_root(mp_int a,
            mp_small b,
            mp_int c);

mp_result
mp_int_set_uvalue(mp_int z,
                  mp_usmall uvalue);

mp_result
mp_int_set_value(mp_int z,
                 mp_small value);

mp_result
mp_int_sqr(mp_int a,
           mp_int c);

mp_result
mp_int_string_len(mp_int z,
                  mp_size radix);

mp_result
mp_int_sub(mp_int a,
           mp_int b,
           mp_int c);

mp_result
mp_int_sub_value(mp_int a,
                 mp_small value,
                 mp_int c);

void
mp_int_swap(mp_int a,
            mp_int c);

mp_result
mp_int_to_binary(mp_int z,
                 unsigned char *buf,
                 int limit);

mp_result
mp_int_to_int(mp_int z,
              mp_small *out);

mp_result
mp_int_to_string(mp_int z,
                 mp_size radix,
                 char *str,
                 int limit);

mp_result
mp_int_to_uint(mp_int z,
               mp_usmall *out);

mp_result
mp_int_to_unsigned(mp_int z,
                   unsigned char *buf,
                   int limit);

mp_result
mp_int_unsigned_len(mp_int z);

void
mp_int_zero(mp_int z);

/* END GENERATED PROTOS */
