#include <kernel/spi-mem.h>

#define SECTOR_SIZE        0x1000
#define PAGE_SIZE   256
#define MOUNT_NAME "/root"
#define MEDIA_PART_TYPE_DATA 15


static_assert(SECTOR_SIZE / PAGE_SIZE < 32,
              "there are more than 32 pages per sector");

int mount_ufs();
int __irq flash_file_open(const char* path, uint8_t mode);
void __irq flash_file_close();
int __irq flash_file_read(uint32_t length, char* data);
void __irq flash_file_gets(uint32_t length, char* data);
void __irq flash_file_write(uint32_t length, char* data);

int mount_fs(uint32_t flash_addr, uint32_t partition_size, const char *mountpt);
int umount_fs();