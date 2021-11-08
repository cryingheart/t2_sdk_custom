#include <bt/bt_gap.h>
void test_server_create(void);
void test_server_destroy(void);
bt_att_error_t test_notification(struct gatt_srv_link *link, uint8_t data);
bt_att_error_t test_indication(struct gatt_srv_link *link, uint8_t data);
