#include "fota.h"
#include "part.h"

int read_part_file(const char *conffile, struct part_config* part_config);
int parse_boot_file(const char *filename);
int parse_ota_file(const char *filename, struct ota_config* fota_config);
int get_image_status(int image_index);
char* get_image_version(int image_index);
void modify_part_file(const char *conffile, struct part_config* part_config);
void modify_boot_file(uint8_t image_num);
void modify_ota_file(struct ota_config* fota_config);
void set_lastboot();
void bootpass();
void inc_bootfail();