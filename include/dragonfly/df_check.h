#pragma once

#include <dragonfly/df_debug.h>

#define CHECK(_fn, ...) do {                                    \
        __auto_type result_ = _fn(__VA_ARGS__);                 \
        if(result_ != df_status_success) {                      \
            df_debug("" #_fn " = %d", result_);                 \
            return result_;                                     \
        }                                                       \
    } while(0)
