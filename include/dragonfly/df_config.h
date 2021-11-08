#pragma once

/* FIXME this doesn't belong here */
#define df_expand(...) __VA_ARGS__
#define df_apply(macro, ...) df_expand(macro(__VA_ARGS__))

#define df_nargs_(a0_, a1_, a2_, count_, ...) count_
#define df_nargs(...) df_expand(df_nargs_(0, ## __VA_ARGS__, 2, 1, 0))

#define df_alloc_n_(n_) df_alloc_##n_
#define df_alloc(...) df_expand(df_apply(df_alloc_n_, df_nargs(__VA_ARGS__))(__VA_ARGS__))

#ifdef __linux__
#include <dragonfly/df_config_linux.h>
#else
#include <dragonfly/df_config_innos.h>
#endif
