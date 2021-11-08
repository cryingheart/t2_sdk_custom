#pragma once

#include <dragonfly/df-cpp-begin.h>

enum dragonfly_status {
    df_status_success,          /* cool and froody */
    df_status_unspecified_failure, /* something went wrong */
    df_status_invalid_argument, /* incorrect or unsupported argument */
    df_status_failed_allocation, /* allocation failures */
    df_status_group_unsupported, /* unsupported ec/fcc group */
    df_status_group_mismatch, /* peers selected different groups */
    df_status_non_quadratic_residue, /* specified value is not square mod p */
    df_status_invalid_scalar,   /* scalar not in range 2..order-1 */
    df_status_invalid_element,  /* element not in range 1..prime-1, or not on curve */
    df_status_invalid_length, /* a protocol message is too short or too long */
    df_status_invalid_shared_secret, /* computed secret is at infinity */
    df_status_invalid_send_confirm, /* */
    df_status_invalid_checksum, /* checksum mismatch */
    df_status_invalid_ac_token, /* anti-clogging token mismatch */
    df_status_missing_ac_token, /* anti-clogging token missing */
    df_status_reflection,       /* potential reflection attack */
};

//#define dragonfly_status dragonfly_status __attribute__((__warn_unused_result__))

#include <dragonfly/df-cpp-end.h>
