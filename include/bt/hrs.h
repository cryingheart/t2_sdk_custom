#include <bt/cmn.h>

#define BT_SENSOR_CONTACT_SUPPORTED           (1<<1)
#define BT_SENSOR_CONTACT_NOT_SUPPORTED       (0<<1)
#define BT_SENSOR_CONTACT_STATUS_DETECTED     (1<<0)
#define BT_SENSOR_CONTACT_STATUS_NOT_DETECTED (0<<0)

typedef enum {
    BT_BSL_OTHER    = 0,
    BT_BSL_CHEST    = 1,
    BT_BSL_WRIST    = 2,
    BT_BSL_FINGER   = 3,
    BT_BSL_HAND     = 4,
    BT_BSL_EAR_LOBE = 5,
    BT_BSL_FOOT     = 6,
    /* 7-255 RESERVED */
} bt_body_sensor_location_t;

typedef bt_att_error_t (*bt_hrs_hrm_cb_t)(uint16_t *bpm, uint8_t *scs, uint16_t *ee, uint8_t *nof_rr, uint16_t rr[16]);
typedef bt_att_error_t (*bt_hrs_bsl_cb_t)(uint8_t *location);
typedef bt_att_error_t (*bt_hrs_hrcp_cb_t)(void);

void hrs_connected(struct gatt_srv_link *gatt);
void hrs_disconnected(void);
bt_gap_error_t hrs_create(const bt_hrs_hrm_cb_t hrm_cb, const bt_hrs_bsl_cb_t bsl_cb, const bt_hrs_hrcp_cb_t hrcp_cb);
void hrs_destroy(void);
