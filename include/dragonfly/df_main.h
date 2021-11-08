#pragma once
#include <stddef.h>
#include <net/ethernet.h>

#include <dragonfly/df_config.h>
#include <dragonfly/df_status.h>
#include <dragonfly/df_debug.h>
#include <dragonfly/df_check.h>

#include <dragonfly/df-cpp-begin.h>

struct dragonfly_peer;
struct dragonfly_instance;

#include <dragonfly/df_cb.h>
#include <dragonfly/df_hash.h>
#include <dragonfly/df_algo.h>

#include <dragonfly/df_mpint.h>

#include <dragonfly/df_protocol.h>
#include <dragonfly/sae_protocol.h>

/* BEGIN GENERATED PROTOS */

void df_random_bytes(unsigned char* data, size_t size);

bool dragonfly_check_sync(struct dragonfly_instance* ctx);

void dragonfly_delete(struct dragonfly_instance* ctx);

enum dragonfly_status
dragonfly_generate_prime_bytes(struct dragonfly_instance* ctx,
                               struct cipher_buffer* result);

size_t dragonfly_get_order_size(struct dragonfly_instance* ctx);

size_t dragonfly_get_prime_size(struct dragonfly_instance* ctx);

uint16_t dragonfly_get_sc(struct dragonfly_instance* ctx);

bool dragonfly_highmac(struct dragonfly_instance* ctx);

void dragonfly_increment_sc(struct dragonfly_instance* ctx);

void dragonfly_increment_sync(struct dragonfly_instance* ctx);

enum dragonfly_status
dragonfly_init(struct dragonfly_instance* ctx,
               const uint8_t host_address[IEEE80211_ADDR_LEN],
               const uint8_t peer_address[IEEE80211_ADDR_LEN]);

struct dragonfly_instance*
dragonfly_new(const uint8_t host_address[IEEE80211_ADDR_LEN],
              const uint8_t peer_address[IEEE80211_ADDR_LEN]);

int
dragonfly_peer_cmp(const struct dragonfly_peer* a,
                   const struct dragonfly_peer* b);

void dragonfly_peer_free(struct dragonfly_peer* peer);

enum dragonfly_status
dragonfly_peer_generate_act(const struct dragonfly_peer* peer,
                            const struct cipher_buffer* key,
                            struct cipher_buffer* token);

void dragonfly_peer_init(struct dragonfly_peer* peer);

enum dragonfly_status
dragonfly_peer_set_act(struct dragonfly_peer* peer,
                       const struct cipher_buffer* act);

void
dragonfly_peer_set_address(struct dragonfly_peer* peer,
                           const uint8_t address[ETHER_ADDR_LEN]);

enum dragonfly_status
dragonfly_peer_set_element(struct dragonfly_peer* peer,
                           const df_ec_point* element);

enum dragonfly_status
dragonfly_peer_set_scalar(struct dragonfly_peer* peer,
                          const df_mpint* scalar);

void dragonfly_reset_sync(struct dragonfly_instance* ctx);

enum dragonfly_status
dragonfly_set_group(struct dragonfly_instance* ctx,
                    enum df_ec_group_id group_id);

enum dragonfly_status
dragonfly_set_password(struct dragonfly_instance* ctx,
                       const void* password,
                       size_t password_length);

enum dragonfly_status
dragonfly_set_password_id(struct dragonfly_instance* ctx,
                          const void* password_id,
                          size_t password_id_length);

void dragonfly_update_rc(struct dragonfly_instance* ctx, uint16_t rc);

void dragonfly_update_sc(struct dragonfly_instance* ctx, uint16_t sc);

enum dragonfly_status
kdf_sha256(void* output,
           size_t length_bits,
           const void* key,
           size_t key_size,
           const void* label,
           size_t label_size,
           const void* context,
           size_t context_size);

/* END GENERATED PROTOS */

#include <dragonfly/df-cpp-end.h>
