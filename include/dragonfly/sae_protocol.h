#pragma once

#include <stddef.h>
#include <stdint.h>
#include <dragonfly/df_mpint.h>
#include <dragonfly/df_hash.h>
#include <dragonfly/df_cb.h>

#include <dragonfly/df-cpp-begin.h>

struct dragonfly_instance;
struct pmksa;

#define SAE_ANTI_CLOGGING_TOKEN_SIZE SHA256_DIGEST_LENGTH

struct sae_commit_message {
    df_ec_group          finite_cyclic_group;  /* 10 */
    struct cipher_buffer anti_clogging_token;  /* 11 */
    struct cipher_buffer scalar;               /* 13 */
    struct cipher_buffer finite_field_element; /* 14 */
    struct cipher_buffer password_identifier;  /* NN */
};

struct sae_confirm_message {
    uint16_t             send_confirm; /* 12 */
    struct cipher_buffer confirm;      /* 15 */
};

enum ieee80211_status_code;

/* BEGIN GENERATED PROTOS */

enum dragonfly_status
sae_generate_pwd_seed(struct dragonfly_instance* ctx,
                      uint8_t counter,
                      struct cipher_buffer* cb);

enum dragonfly_status
sae_generate_pwd_value(struct dragonfly_instance* ctx,
                       const struct cipher_buffer* prime,
                       const struct cipher_buffer* password_seed,
                       df_mpint* password_value);

enum dragonfly_status
sae_generate_keyseed(struct dragonfly_instance* ctx,
                     const df_mpint* secret,
                     uint8_t keyseed[SHA256_DIGEST_LENGTH]);

enum dragonfly_status
sae_derive_keys(struct dragonfly_instance* ctx,
                const df_mpint* secret);

enum dragonfly_status
sae_get_pmksa(struct dragonfly_instance* ctx,
              struct pmksa* pmksa);

enum dragonfly_status sae_generate_act(struct dragonfly_instance* ctx);

void sae_commit_message_init(struct sae_commit_message* cm);

void sae_commit_message_free(struct sae_commit_message* cm);

enum dragonfly_status
sae_parse_commit_message(struct dragonfly_instance* ctx,
                         enum ieee80211_status_code status,
                         const void* message,
                         size_t length,
                         struct sae_commit_message* cm);

enum dragonfly_status
sae_commit_message_check_group(struct dragonfly_instance* ctx,
                               const struct sae_commit_message* cm);

enum dragonfly_status
sae_validate_scalar_element(struct dragonfly_instance* ctx,
                            const df_mpint* scalar,
                            const df_ec_point* element);

enum dragonfly_status
sae_validate_commit_message(struct dragonfly_instance* ctx,
                            enum ieee80211_status_code status,
                            const struct sae_commit_message* cm);

enum dragonfly_status
sae_build_commit_message(struct cipher_buffer* commit_message,
                         uint16_t group_id,
                         const struct cipher_buffer* act,
                         size_t prime_size,
                         const df_mpint* scalar,
                         const df_ec_point* element,
                         const struct cipher_buffer* password_id);

enum dragonfly_status
sae_generate_commit_message(struct dragonfly_instance* ctx,
                            struct cipher_buffer* cm);

void sae_confirm_message_init(struct sae_confirm_message* cm);

void sae_confirm_message_free(struct sae_confirm_message* cm);

enum dragonfly_status
sae_parse_confirm_message(struct dragonfly_instance* ctx,
                          enum ieee80211_status_code status,
                          const void* message,
                          size_t length,
                          struct sae_confirm_message* cm);

enum dragonfly_status
sae_validate_send_confirm(struct dragonfly_instance* ctx,
                          uint16_t send_confirm);

enum dragonfly_status
sae_validate_confirm_message(struct dragonfly_instance* ctx,
                             enum ieee80211_status_code status,
                             struct sae_confirm_message* cm);

enum dragonfly_status
sae_generate_confirm_message(struct dragonfly_instance* ctx,
                             struct cipher_buffer* cm);

/* END GENERATED PROTOS */

#include <dragonfly/df-cpp-end.h>
