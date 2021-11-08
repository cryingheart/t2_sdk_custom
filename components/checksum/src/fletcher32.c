#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kernel/os.h>
#include <kernel/page_alloc.h>
#include <kernel/spi.h>
#include <kernel/spi-mem.h>
#include <kernel/hwreg.h>
#include <kernel/elf.h>
#include <kernel/boot.h>
#include <kernel/uart.h>
#include <kernel/secureboot.h>
#include <kernel/packet.h>
#include <kernel/uart.h>
#include <kernel/flash.h>
#include <block/mtd.h>
#include <sys/mount.h>
#include <dirent.h>
#include <inttypes.h>

//#define FL32_DEBUG_ENABLE

#ifdef FL32_DEBUG_ENABLE
#define fl32_printf(_fmt, ...)   os_printf(_fmt, ##__VA_ARGS__)
#else
#define fl32_printf(_fmt, ...)
#endif

/**
 * The fletcher32 routine is from wiki:
 * https://en.wikipedia.org/wiki/Fletcher%27s_checksum
 */
unsigned int fletcher32(const unsigned short *data, size_t len) {
    unsigned int c0, c1;
    len = (len + 1) & ~1;      /* Round up len to words */

    /* We similarly solve for n > 0 and n * (n+1) / 2 * (2^16-1) < (2^32-1) 
       here. */
    /* On modern computers, using a 64-bit c0/c1 could allow a group size of 
       23726746. */
    for (c0 = c1 = 0; len > 0; ) {
        size_t blocklen = len;
        if (blocklen > 360*2) {
            blocklen = 360*2;
        }
        len -= blocklen;
        do {
            c0 = c0 + *data++;
            c1 = c1 + c0;
        } while ((blocklen -= 2));
        c0 = c0 % 65535;
        c1 = c1 % 65535;
    }
    return (c1 << 16 | c0);
}

/**
 * Return checksum
 */
inline unsigned int 
fletcher32_checksum_get(char *buff, unsigned int size)
{
    return  fletcher32((const unsigned short *)buff,size);
}

/**
 * Return -1 if failure. The checksum is returned through checksum_out
 */
int 
fletcher32_file_checksum_get(const char *path, unsigned int *checksum_out)
{
    FILE *f;
    int size, nread;
    char *buff;
    unsigned int chk_sum = 0;
    
    f = fopen(path,"r");
    if(f == NULL){
        fl32_printf("\nError: Failed opening %s\n", path);
        return -1;
    }
    fseek(f, 0, SEEK_END);
    size = ftell(f);/*Calculate the file size*/
    fseek(f, 0, SEEK_SET);
    
    buff = os_alloc(size + 1);
    if(NULL == buff){
        fl32_printf("\nError: malloc failed @%d\n", __LINE__);
        fclose(f);
        return -1;
    }
    nread = fread(buff, 1, size, f);
    if(nread != size){
        fl32_printf("\nError: %s Read failed size = %d nread = %d\n",
                    path, size, nread);
        return -1;
    }
    buff[size] = 0;
    chk_sum = fletcher32((const unsigned short *)buff,size);
    *checksum_out = chk_sum;
    os_free(buff);
    fclose(f);
    return 0;
}

