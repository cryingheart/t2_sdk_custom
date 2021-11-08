/**
 * @addtogroup wifi_core
 * @{
 */
#pragma once
#include <stdint.h>

/**
 * WiFi rate representation
 */
typedef unsigned short rate_t;

#define _RATE(_format, _index) \
    (((_format) << 8) | ((_index) << 0))

#define RATE_FMT_11B 0
#define RATE_FMT_11G 1
#define RATE_FMT_11N 2

/* used to get the basic modulation scheme of this rate, ignoring all
 * options that may exist */
#define RATE_FUNDAMENTAL_MASK 0x307

#define RATE_FLAG_RESERVED_3     BIT(3)
#define RATE_FLAG_RESERVED_4     BIT(4)
#define RATE_FLAG_RESERVED_5     BIT(5)
#define RATE_FLAG_RESERVED_6     BIT(6)
#define RATE_FLAG_RESERVED_7     BIT(7)
#define RATE_FLAG_RESERVED_10    BIT(10)
#define RATE_FLAG_UNUSED_11      BIT(11)
#define RATE_FLAG_UNUSED_12      BIT(12)
#define RATE_FLAG_BASIC          BIT(13)
#define RATE_FLAG_SHORT_PREAMBLE BIT(14)
#define RATE_FLAG_SHORT_GI       BIT(15)

#define RATE_1     _RATE(RATE_FMT_11B, 0)
#define RATE_2     _RATE(RATE_FMT_11B, 1)
#define RATE_5_5   _RATE(RATE_FMT_11B, 2)
#define RATE_11    _RATE(RATE_FMT_11B, 3)

#define RATE_6     _RATE(RATE_FMT_11G, 0)
#define RATE_9     _RATE(RATE_FMT_11G, 1)
#define RATE_12    _RATE(RATE_FMT_11G, 2)
#define RATE_18    _RATE(RATE_FMT_11G, 3)
#define RATE_24    _RATE(RATE_FMT_11G, 4)
#define RATE_36    _RATE(RATE_FMT_11G, 5)
#define RATE_48    _RATE(RATE_FMT_11G, 6)
#define RATE_54    _RATE(RATE_FMT_11G, 7)

#define RATE_HT20_0 _RATE(RATE_FMT_11N, 0)
#define RATE_HT20_1 _RATE(RATE_FMT_11N, 1)
#define RATE_HT20_2 _RATE(RATE_FMT_11N, 2)
#define RATE_HT20_3 _RATE(RATE_FMT_11N, 3)
#define RATE_HT20_4 _RATE(RATE_FMT_11N, 4)
#define RATE_HT20_5 _RATE(RATE_FMT_11N, 5)
#define RATE_HT20_6 _RATE(RATE_FMT_11N, 6)
#define RATE_HT20_7 _RATE(RATE_FMT_11N, 7)

#define RATE_INVALID 0xffff

#define IEEE80211_RATE_SIZE     8 /* 802.11 standard */

/*
 * structure representing a set of rates
 */
#define WIFI_RATESET_MAXSIZE    24

struct rateset {
    uint8_t nrates;
    rate_t rates[WIFI_RATESET_MAXSIZE];
};

static inline unsigned int rate_to_format(rate_t rate)
{
    return (rate >> 8) & 3;
}

static inline unsigned int rate_to_index(rate_t rate)
{
    return (rate >> 0) & 255;
}

/* this wastes four entries for the unused 11b rate indicies, but is
 * probably worth the simplicity */
#define RATE_ARRAY_SIZE (8+8+8)

static inline unsigned int rate_to_array_index(rate_t rate)
{
    return 8 * rate_to_format(rate) + rate_to_index(rate);
}

static inline rate_t rate_from_array_index(unsigned int array_index)
{
    return _RATE(array_index / 8, array_index % 8);
}

struct ieee80211_ie_htcap;
struct ieee80211_ie_htoper;

rate_t rate_from_ieee(uint8_t rate);

rate_t rate_get_mandatory(rate_t ref);

unsigned int rate_linkspeed(rate_t rate);

unsigned int
rateset_add_from_ht_capabilities(struct rateset* rateset,
                                 const struct ieee80211_ie_htcap* cap);

unsigned int
rateset_add_from_ht_operation(struct rateset* rateset,
                              const struct ieee80211_ie_htoper* op);

unsigned int
rateset_add_from_supported_rates(struct rateset* rateset,
                                 const uint8_t* ie);

int rateset_add_rate(struct rateset* rateset, rate_t rate);

void rateset_filter_rates(struct rateset *rateset, int (*include_rate)(rate_t rate));

unsigned int
wifi_rate_get_basic(unsigned fmt,
                    const struct rateset* set);

/** @} */
