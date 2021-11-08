#pragma once

#include <dragonfly/df_mpint.h>

#include <dragonfly/df-cpp-begin.h>

struct dragonfly_instance;
struct dragonfly_peer;

/* BEGIN GENERATED PROTOS */

enum dragonfly_status
dragonfly_build_pe(struct dragonfly_instance* ctx,
                   df_ec_point* pwe);

enum dragonfly_status
dragonfly_generate_pe(struct dragonfly_instance* ctx);

#if defined(__linux__)
void
df_clone_random_numbers(const df_mpint* rand,
                        const df_mpint* mask,
                        size_t size);
#endif

enum dragonfly_status
dragonfly_build_commit_element(struct dragonfly_instance* ctx,
                               struct dragonfly_peer* peer);

enum dragonfly_status
dragonfly_generate_commit_element(struct dragonfly_instance* ctx);

enum dragonfly_status
dragonfly_validate_scalar(const df_ec_group* group,
                          const df_mpint* scalar);

enum dragonfly_status
dragonfly_validate_peer_scalar(struct dragonfly_instance* ctx);

enum dragonfly_status
dragonfly_validate_element(const df_ec_group* group,
                           const df_ec_point* element);

enum dragonfly_status
dragonfly_validate_peer_element(struct dragonfly_instance* ctx);

enum dragonfly_status
dragonfly_compute_shared_secret(struct dragonfly_instance* ctx,
                                df_mpint* secret);

/* END GENERATED PROTOS */

#include <dragonfly/df-cpp-end.h>
