#pragma once

#include <kernel/os.h>
#include <kernel/endian.h>

#define df_alloc_1(type_)         os_alloc(sizeof(type_))
#define df_alloc_2(type_, count_) os_calloc(sizeof(type_), (count_))
#define df_free                   os_free
#define df_printf(...) os_printf(__VA_ARGS__)

#define df_htole16      htole16
#define df_le16toh      le16toh

#define df_array_size   ARRAY_SIZE
#define df_array_foreach ARRAY_FOREACH
#define df_howmany      howmany

#include <wifi/ieee80211.h>
#include <wifi/pmksa.h>
