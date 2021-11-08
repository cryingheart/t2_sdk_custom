#include <kernel/os.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <kernel/os.h>
#include <kernel/spi-mem.h>
#include <kernel/flash.h>
#include <kernel/boot.h>
#include <kernel/callout.h>
#include <kernel/io.h>
#include <kernel/hwreg.h>
#include <kernel/watchdog.h>
#include <wifi/wcm.h>
#include <block/mtd.h>
#include <sys/mount.h>

void print_ver(char *banner, int print_sdk_name, int print_emb_app_ver)
{
    if(print_emb_app_ver == 1) {
        os_printf("$App:git-%s\n", EMB_APP_GIT_ID);
    }
    if(print_sdk_name == 1) {
        os_printf("SDK Ver: %s\n", SDK_BRANCH_NAME);
    }
    if(banner != NULL) {
        os_printf("%s\n", banner);
    }
}

/**
 * Print Application Inforamation.
 */
void
print_app_info(char *name, char *version)
{
    size_t heap;
    os_printf("\n");
    os_printf("\nApplication Information:");
    os_printf("\n------------------------");
    os_printf("\nName       : %s", name);
    os_printf("\nVersion    : %s", version);
    os_printf("\nBuild Date : %s", __DATE__);
    os_printf("\nBuild Time : %s", __TIME__);
    heap = os_avail_heap();
    os_printf("\nHeap Available: %d KB (%d Bytes)", heap / 1024, heap);
    os_printf("\n");
}

/**
 * Reset the device.
 */
void
reset_device(void)
{
    uint8_t trip = 0;
    mmio_wr8(1, &hw_wdg->unlock);
    mmio_wr8(1, &hw_wdg->run);
    mmio_wr8_set(PMU_RST_MASK_POR, &hw_pmu->rst_mask[1]);
    mmio_wr8_clr(PMU_RST_MASK_POR, &hw_pmu->rst_mask[2]);
    trip |= WDG_TRIP_RESET;
    mmio_wr8(trip, &hw_wdg->trip);
    mmio_wr16(0, &hw_wdg->preload);
    while(1);
}


/**
 * Show heap available. This will autom
 */
void
show_heap(char *function, char *line_num)
{
    size_t heap;
    heap = os_avail_heap();

    os_printf("Free heap at {fn: %s ln:%d} = %d KB (%d Bytes)\n",
              __FUNCTION__, __LINE__,
              heap / 1024, heap);

}

/**
 * DumpHex function
 *
 * @param data Data to hexdump-ed
 * @param size Size of data
 * @param show_string whether to print the string data or not
 */
void DumpHex(char* description, const void* data, size_t size, int show_string)
{
    char ascii[17];
    size_t i, j;
    os_printf("Hexdump of %s, len=%d\n", description, (int)size);
    ascii[16] = '\0';
    for (i = 0; i < size; ++i) {
        os_printf("%02X ", ((unsigned char*)data)[i]);
        if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
            ascii[i % 16] = ((unsigned char*)data)[i];
        } else {
            ascii[i % 16] = '.';
        }
        if ((i + 1) % 8 == 0 || i + 1 == size) {
            os_printf(" ");
            if ((i + 1) % 16 == 0) {
                if(show_string == 1)
                    os_printf("|  %s \n", ascii);
                else
                    os_printf("||\n");
            } else if (i + 1 == size) {
                ascii[(i + 1) % 16] = '\0';
                if ((i + 1) % 16 <= 8) {
                    os_printf(" ");
                }
                for (j = (i + 1) % 16; j < 16; ++j) {
                    os_printf("   ");
                }
                if(show_string == 1)
                    os_printf("|  %s \n", ascii);
                else
                    os_printf("||\n");
            }
        }
    }
}
/*
 * Compares 2 num strings like "1.1" and "1.2" and returns val
 * similar to strcmp
 * 0 - if the number of both the strigs are equal 
 *      e.g: "1.1" and "01.1" are equal.
 * ret > 0 - if number of str1 > number of str2
 * ret < 0 - if number of str1 < number of str2
*/
int utils_num_str_cmp(const char *str1, const char *str2)
{
    int deci1 = 0, deci2 = 0, fracn1 = 0, fracn2 = 0;
    os_printf("\n%s\n", __FUNCTION__);
    while(*str1 && *str1 != '.'){
        deci1 = (deci1 * 10) + *str1 - '0';
        os_printf(" %d", deci1);
        str1++;

    }
    os_printf("\r\n");
    if(*str1 == '.') str1++;
    while(*str1){        
        fracn1 = (fracn1 * 10) + *str1 - '0';
         os_printf(" %d", fracn1);
        str1++;
    }
    os_printf("\r\n");
    while(*str2 && *str2 != '.'){
        deci2 = (deci2 * 10) + *str2 - '0';
        os_printf(" %d", deci2);
        str2++;
    }
    os_printf("\r\n");
    if(*str2 == '.') str2++;
    while(*str2){
        fracn2 = (fracn2 * 10) + *str2 - '0';
        os_printf(" %d", fracn2);
        str2++;
    }
    os_printf("\r\n");
    os_printf("deci1 = %d, fracn1 = %d, deci2 = %d, fracn2 = %d\r\n", 
               deci1, fracn1, deci2, fracn2);
    if(deci1 == deci2){
        if(fracn1 == fracn2)
            return 0;
        return (fracn1 - fracn2);
    }
        
    return (deci1 - deci2);
}

/*
 * Reverse a string
*/
char* reverse_str(char *s)
{
    const int len = strlen(s);
    const int mid = len / 2;
    for (int i = 0, j=len-1; i < mid; i++, j--) {
        char c = s[i];
        s[i]   = s[j];
        s[j]   = c;
    }
    return s;
}
