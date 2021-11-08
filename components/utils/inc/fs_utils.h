#define ROOT_FS_ADDR	0x1c0000
#define ROOT_FS_SIZE	0x40000

int utils_mount_rootfs(void);

int utils_umount_rootfs(void);

int utils_file_size_get(const char * path);

char * utils_file_get(const char * path, int *len);

int 
utils_file_store(const char *file_path, char *buff, int len);
int
utils_file_clone(const char *dst_path, const char *src_path);
int 
utils_create_checksum_file(const char *file_path, const char *chksum_file_path);
int 
utils_create_checksum_file(const char *file_path, const char *chksum_file_path);
int utils_file_touch(const char *path);
int utils_file_absolute_path_get(char *path, char *abs_path_out);
int
utils_is_file_present(const char * path);
int 
utils_update_fota_cfg_file(char *buf, int len);


