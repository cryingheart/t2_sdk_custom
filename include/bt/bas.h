#include <bt/cmn.h>

typedef bt_att_error_t (*bt_bas_level_cb_t)(uint8_t *level);

bt_gap_error_t bas_create(const bt_bas_level_cb_t level_cb);
void bas_destroy(void);
