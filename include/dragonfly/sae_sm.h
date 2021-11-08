#pragma once

#include <stddef.h>
#include <stdint.h>

#include <dragonfly/df_config.h>

#include <dragonfly/df_ecc.h>

#include <dragonfly/df-cpp-begin.h>

struct dragonfly_instance;

enum sae_sm_status {
    sae_sm_status_continue,                /* in progress, more to come */
    sae_sm_status_auth,                    /* successfully authenticated */
    sae_sm_status_fail_bad_status,         /* unexpected status code */
    sae_sm_status_fail_incompatible,       /* incompatble settings */
    sae_sm_status_fail_internal,           /* unspecified internal error */
    sae_sm_status_fail_validation,         /* failed to validate received frame */
    sae_sm_status_fail_try_count_exceeded, /* too many attempts */
    sae_sm_status_fail_timeout,            /* protocol timeout */
};

struct sae_sm_state;

struct sae_sm_callbacks {
    int (*send_auth_frame)(const void*, const void*, size_t);
    int (*send_deauth_frame)(const void*, enum ieee80211_reason_code reason);
    int (*request_timeout)(const void*, uint32_t msec);
    int (*delete_event)(const void*);
};

/* BEGIN GENERATED PROTOS */

enum dragonfly_status
sae_sm_state_enable_group(struct sae_sm_state* sm,
                          enum df_ec_group_id gid);

enum dragonfly_status
sae_sm_state_disable_group(struct sae_sm_state* sm,
                           enum df_ec_group_id gid);

struct sae_sm_state*
sae_sm_state_new(struct dragonfly_instance* instance,
                 const struct sae_sm_callbacks* cb,
                 const void* handle);

void sae_sm_state_delete(struct sae_sm_state* sm);

enum sae_sm_status sae_sm_state_start(struct sae_sm_state* sm);

enum sae_sm_status sae_sm_state_timeout(struct sae_sm_state* sm);

enum sae_sm_status
sae_sm_state_rx_auth(struct sae_sm_state* sm,
                     const void* body,
                     size_t length);

/* END GENERATED PROTOS */

#include <dragonfly/df-cpp-end.h>
