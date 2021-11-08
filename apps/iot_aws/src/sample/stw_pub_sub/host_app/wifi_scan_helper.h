#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define IEEE80211_NWID_LEN 32
#define IEEE80211_EID_SSID 0
#define ELEMENT_ID 0
#define ELEMENT_LENGTH 1
#define ELEMENT_INFORMATION 2
#define EINVAL 22
#define ENOENT 2


struct ie_iterator {
    const uint8_t *buf;
    const uint8_t *pos;
    const uint8_t *limit;
};

static inline void
ie_iter_init(struct ie_iterator *iter, const uint8_t *ie_list, size_t len)
{
    iter->buf = ie_list;
    iter->pos = ie_list;
    iter->limit = ie_list + len;
}

static inline void
ie_iter_reset(struct ie_iterator *iter)
{
    iter->pos = iter->buf;
}

static inline bool
ie_iter_end(const struct ie_iterator *iter)
{
    return iter->pos - iter->limit >= 0;
}

static inline const uint8_t*
ie_iter_ptr(const struct ie_iterator *iter)
{
    if (ie_iter_end(iter))
        return NULL;
    return &iter->pos[ELEMENT_ID];
}

static inline unsigned int
ie_iter_id(const struct ie_iterator *iter)
{
    if (ie_iter_end(iter))
        return -1;
    return iter->pos[ELEMENT_ID];
}

static inline unsigned int
ie_iter_len(const struct ie_iterator *iter)
{
    if (ie_iter_end(iter))
        return 0;
    return iter->pos[ELEMENT_LENGTH];
}

static inline const uint8_t *
ie_iter_value(const struct ie_iterator *iter)
{
    return &iter->pos[ELEMENT_INFORMATION];
}

static inline void
ie_iter_next(struct ie_iterator *iter)
{
    if (!ie_iter_end(iter))
        iter->pos += 2 + ie_iter_len(iter);
}

static inline bool
ie_iter_find(struct ie_iterator *iter, unsigned id)
{
    ie_iter_reset(iter);

    while (!ie_iter_end(iter)) {
        if (ie_iter_id(iter) == id)
            return true;
        ie_iter_next(iter);
    }
    return false;
}

struct wifi_ssid {
    uint8_t ws_len;
    /* an SSID should be treated as binary data, but since much code
     * assumes it to be a string, we leave room for zero
     * termination */
    char    ws_ssid[IEEE80211_NWID_LEN+1];
};

/**
 * @brief Initialize SSID from a byte string.
 *
 * This function will initialize an SSID structure from a byte
 * string. An SSID is a sequence of bytes, not
 * always representable as a zero terminated string. This function
 * will always generate a zero-terminated result, but treating it as
 * such will cause problems with SSID's with embedded zero bytes
 * (although this is uncommon). If the input byte sequence is too long
 * (more than IEEE80211_NWID_LEN bytes), this function will truncate
 * the value, and return a negative result.
 *
 * @param ssid structure to initialize
 * @param ssid_bytes pointer to SSID data
 * @param ssid_length length of @a ssid_bytes
 *
 * @retval zero on success
 * @retval -EINVAL @p ssid_length exceeds IEEE80211_NWID_LEN
 */
int wifi_ssid_from_bytes(struct wifi_ssid *ssid,
                         const void *ssid_bytes,
                         size_t ssid_length)
{
    int retval = 0;
    if(ssid_length > IEEE80211_NWID_LEN) {
        ssid_length = IEEE80211_NWID_LEN;
        printf("EINVAL\n");
        retval = -EINVAL;
    }
    ssid->ws_len = ssid_length;
    memcpy(ssid->ws_ssid, ssid_bytes, ssid_length);
    ssid->ws_ssid[ssid->ws_len] = '\0';
    return retval;
}

/**
 * @brief Initialize SSID from an information element.
 *
 * If the provided IE iterator points to a valid SSID information
 * element, it will call @c wifi_ssid_from_bytes on the content of
 * said element. If not it will initialise it to the empty SSID, and
 * return -ENOENT.
 *
 * @param ssid structure to initialize
 * @param ie IE iterator
 *
 * @retval zero on success
 * @retval -EINVAL @p ie exceeds IEEE80211_NWID_LEN
 * @retval -ENOENT @p ie does not point to an SSID element
 */
int wifi_ssid_from_ie(struct wifi_ssid *ssid,
                      struct ie_iterator *ie)
{
    if(ie_iter_id(ie) != IEEE80211_EID_SSID) {
        wifi_ssid_from_bytes(ssid, "", 0);
        return -ENOENT;
    }
    return wifi_ssid_from_bytes(ssid, ie_iter_value(ie), ie_iter_len(ie));
}

/* Extract SSID from NI.
 *
 * @ retval 0 on success
 * @ retval -EINVAL SSID element is too long
 * @ retval -ENOENT SSID element is missing
 */
int wifi_netinfo_get_ssid(const uint8_t *ie_list, size_t len, struct wifi_ssid *ssid)
{
    struct ie_iterator iter;

    ie_iter_init(&iter, ie_list, len);

    ie_iter_find(&iter, IEEE80211_EID_SSID);

    return wifi_ssid_from_ie(ssid, &iter);
}