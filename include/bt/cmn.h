#include <bt/bt_gap.h>
void common_server_create(char *name, uint16_t appearance, char *manufacturer_name);
void common_server_destroy(void);
bt_att_error_t service_changed_indication(struct gatt_srv_link *link, uint16_t handle_start, uint16_t handle_end);
