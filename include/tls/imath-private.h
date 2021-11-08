#pragma once

/* BEGIN GENERATED PROTOS */

void
s_2comp(unsigned char *buf,
        int len);

int
s_2expt(mp_int z,
        mp_small k);

mp_digit *s_alloc(mp_size num);

mp_result
s_brmu(mp_int z,
       mp_int m);

int
s_cdig(mp_digit *da,
       mp_digit *db,
       mp_size len);

int
s_ch2val(char c,
         int r);

void
s_dadd(mp_int a,
       mp_digit b);

void
s_dbmul(mp_digit *da,
        mp_digit b,
        mp_digit *dc,
        mp_size size_a);

mp_digit
s_ddiv(mp_int a,
       mp_digit b);

void
s_dmul(mp_int a,
       mp_digit b);

int
s_dp2k(mp_int z);

mp_result
s_embar(mp_int a,
        mp_int b,
        mp_int m,
        mp_int mu,
        mp_int c);

void
s_fake(mp_int z,
       mp_small value,
       mp_digit vbuf[]);

void
s_free(void *ptr);

mp_size
s_inlen(int len,
        mp_size r);

int
s_isp2(mp_int z);

int
s_kmul(mp_digit *da,
       mp_digit *db,
       mp_digit *dc,
       mp_size size_a,
       mp_size size_b);

int
s_ksqr(mp_digit *da,
       mp_digit *dc,
       mp_size size_a);

int
s_norm(mp_int a,
       mp_int b);

int
s_outlen(mp_int z,
         mp_size r);

int
s_pad(mp_int z,
      mp_size min);

void
s_print(char *tag,
        mp_int z);

void
s_print_buf(char *tag,
            mp_digit *buf,
            mp_size num);

void
s_qdiv(mp_int z,
       mp_size p2);

void
s_qmod(mp_int z,
       mp_size p2);

int
s_qmul(mp_int z,
       mp_size p2);

int
s_qsub(mp_int z,
       mp_size p2);

mp_digit *s_realloc(mp_digit *old,
mp_size osize,
mp_size nsize);

int
s_reduce(mp_int x,
         mp_int m,
         mp_int mu,
         mp_int q1,
         mp_int q2);

mp_result
s_tobin(mp_int z,
        unsigned char *buf,
        int *limpos,
        int pad);

mp_digit
s_uadd(mp_digit *da,
       mp_digit *db,
       mp_digit *dc,
       mp_size size_a,
       mp_size size_b);

int
s_ucmp(mp_int a,
       mp_int b);

mp_result
s_udiv_knuth(mp_int u,
             mp_int v);

void
s_ufake(mp_int z,
        mp_usmall value,
        mp_digit vbuf[]);

void
s_usqr(mp_digit *da,
       mp_digit *dc,
       mp_size size_a);

void
s_usub(mp_digit *da,
       mp_digit *db,
       mp_digit *dc,
       mp_size size_a,
       mp_size size_b);

int
s_uvcmp(mp_int a,
        mp_usmall uv);

int
s_uvpack(mp_usmall uv,
         mp_digit t[]);

char
s_val2ch(int v,
         int caps);

int
s_vcmp(mp_int a,
       mp_small v);

/* END GENERATED PROTOS */
