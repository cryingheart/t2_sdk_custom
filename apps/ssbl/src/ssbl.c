#include <stdio.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

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

#include "json/basic_json/json.h" 
#include "utils/inc/utils.h"
#include "utils/inc/fs_utils.h"
#include "utils/inc/fs_defines.h"
#include "checksum/inc/fletcher32.h"

//#define SSBL_DEBUG_ENABLE
#define SSBL_PRINT_BOOTARGS

#define SSBL_FLASH_SCTOR_SIZE       4096
#define SSBL_SECTION_NAMES_SZE_MAX  1024
#define SSBL_ERROR      -1
#define SSBL_SUCCESS    0

enum{
    JSON_PARSE_STATE_KEY,
    JSON_PARSE_STATE_SEPRATOR,
    JSON_PARSE_STATE_VAL
};

enum ssb_sanity_e{
    SSB_SANITY_ERROR_NONE,
    SSB_SANITY_ERROR_MASTER_COPY,
    SSB_SANITY_ERROR_BACKUP_COPY,
};

/** @internal
 *
 * Data Structure used to store the image information from the part.json file 
 * file 
 */
typedef struct part_image_info{
    int index;          /**<index in the image array in part.json file*/
    char name[64];      
    char version[64];   /**< FOTA version. This is not the Firmware version*/
    int sector;         /**< Starting sector*/
    char *boot_args[32];
    int boot_args_cnt;
    struct part_image_info *next;
}part_image_info_t;

typedef struct{
    part_image_info_t *image_info_list;    
}part_file_t;

part_file_t part_file;

char *boot_arg_p = (char*)0xbfffc;
struct boot_args image_boot_args;
int vm_flash_location;
uint32_t debugen_save;

#ifdef SSBL_DEBUG_ENABLE
#define ssbl_printf(_fmt, ...)   os_printf(_fmt, ##__VA_ARGS__)
#else
#define ssbl_printf(_fmt, ...)
#endif

static void __naked
entry_trampoline(uint32_t reason)
{
    if (debugen_save)
        mmio_wr32(DHCSR_DBGKEY | DHCSR_C_DEBUGEN, &arm_scb->dhcsr);
    image_boot_args.entry(image_boot_args.argc, image_boot_args.argv);
}

void os_early_alloc_hook()
{
    os_page_claim((void*)0x41b00, (void*)0x80000);
}

void ssbl_build_argv(int ac, char **av, char *ptr)
{
    for(int i=0; i<ac; i++){
        av[i]=ptr;
        while(*ptr++);
    }
}

void ssbl_start_image(void)
{
    ssbl_printf("\nStarting image...\n");

    uintptr_t addr = (uintptr_t)boot_arg_p - 4* image_boot_args.argc;
    image_boot_args.argv = (void*)(addr & ~3);

    /* Build boot args to be passed to the booting application
     */
    ssbl_build_argv(image_boot_args.argc, image_boot_args.argv, boot_arg_p);
#ifdef SSBL_PRINT_BOOTARGS
    os_printf("\nBoot-args:\n");
    for(int i=0; i<image_boot_args.argc; i++)
        os_printf("\t%s\n", image_boot_args.argv[i]);
#endif

    /* Setup resume handler */
    mmio_wr32((uint32_t)entry_trampoline, &hw_pmu->reserved[0]);
    debugen_save = mmio_rd32(&arm_scb->dhcsr) & DHCSR_C_DEBUGEN;

    /* kick reset */
    os_printf("\nResetting...");
    os_printf("\n=====================================================\n\n\n");
    os_msleep(10);
    os_console_flush();
    mmio_wr8_clr(PMU_CLOCK_SEL_XO, &hw_pmu->clock_sel);
    mmio_wr8(WDG_TRIP_RESET, &hw_wdg->trip);
    mmio_wr8(1, &hw_wdg->unlock);
    mmio_wr8(1, &hw_wdg->run);

    for(;;);
}

void ssbl_read_section(struct spi_mem_device *dev, uint32_t flash_addr, 
                                uint32_t data_addr, size_t len)
{
    uint8_t *data_ptr;
    size_t chunk;
    if(0 == data_addr){      
        return;
    }
    data_ptr = (uint8_t *)data_addr;     
    while(len) {
        chunk = len > SSBL_FLASH_SCTOR_SIZE? SSBL_FLASH_SCTOR_SIZE: len;
        spi_mem_read(dev, flash_addr, data_ptr, chunk);
        flash_addr += chunk;
        data_ptr   += (data_addr) ? chunk : 0;
        len -= chunk;
    }
}

/*
        -----------------------------
        |                           |
        |    ELF Header             |
        -----------------------------
        |                           |
        |    program Header Table   |
        -----------------------------
        |    .text                  |
        -----------------------------
        |    .rodata                |
        -----------------------------
        |                           |
        |    .....                  |
        -----------------------------
        |                           |        
        |    .data                  |
        -----------------------------
        |                           |
        |    section header table   |
        -----------------------------

void set_signature(char* arg)
{
    memcpy(&config.signature, arg, 64);
}

*/

int ssbl_load_elf(uint32_t flash_addr)
{
    Elf32_Ehdr ehdr;
    Elf32_Phdr phdr;
    Elf32_Shdr shdr;
    Elf32_Off offset;
    unsigned int i, app_size = 0;

    char *section_names = os_alloc(SSBL_SECTION_NAMES_SZE_MAX); 
    if(NULL == section_names){
        os_printf("\nError : Malloc failed @ %x",__LINE__);
        return SSBL_ERROR;
    }
    struct spi_mem_device * spi_mem = os_flash_get_spi_dev();

    ssbl_printf("\n%s : Read elf @ %x", __FUNCTION__, flash_addr);
    spi_mem_read(spi_mem, flash_addr, &ehdr, sizeof(ehdr));
    if(memcmp(ehdr.e_ident, "\x7f""ELF\x01\x01", 6) != 0)
        return SSBL_ERROR;
    if(ehdr.e_version != EV_CURRENT)
        return SSBL_ERROR;
    if(ehdr.e_ehsize != sizeof(ehdr))
        return SSBL_ERROR;
    if(ehdr.e_type != ET_EXEC)
        return SSBL_ERROR;
    if(ehdr.e_machine != EM_ARM)
        return SSBL_ERROR;
    if(ehdr.e_phentsize != sizeof(phdr))
        return SSBL_ERROR;
    ssbl_printf("\nElf OK.");
    /* Read the section names */
    offset = ehdr.e_shoff + ehdr.e_shstrndx*sizeof(shdr);
    spi_mem_read(spi_mem, flash_addr+offset, &shdr, sizeof(shdr));
    ssbl_printf("\nReading section names @ offset = %x", shdr.sh_offset);
    spi_mem_read(spi_mem, flash_addr+shdr.sh_offset, section_names, shdr.sh_size);

    /* Load application into RAM */    
    offset = ehdr.e_shoff;
    app_size = 0;
    for(i = 0; i < ehdr.e_shnum; i++) {
        spi_mem_read(spi_mem, flash_addr+offset, &shdr, sizeof(shdr));
        ssbl_printf("\n%s", &section_names[shdr.sh_name]);
        if (!strcmp(&section_names[shdr.sh_name], ".text") ||
            !strcmp(&section_names[shdr.sh_name], ".data") ||
            !strcmp(&section_names[shdr.sh_name], ".text.bvars") ||
            !strcmp(&section_names[shdr.sh_name], ".text.base") ||
            !strcmp(&section_names[shdr.sh_name], ".atext") ||
            !strcmp(&section_names[shdr.sh_name], ".data.base") ||
            !strcmp(&section_names[shdr.sh_name], ".ARM.attributes") ||
            !strcmp(&section_names[shdr.sh_name], ".bss") ||
            !strcmp(&section_names[shdr.sh_name], ".bss.base")) {
            /* Copy these sections in to RAM
             */
            app_size += shdr.sh_size;
            if(app_size >= 0x3e000){
                os_printf("\nError: Application is too big - %x", app_size);
                return SSBL_ERROR;
            }
            ssbl_read_section(spi_mem,flash_addr+shdr.sh_offset, shdr.sh_addr, shdr.sh_size);
        }
        if (strcmp(&section_names[shdr.sh_name], ".virt") == 0) {
            /* No need to Copy .virt section into RAM
             */
            vm_flash_location=(flash_addr+shdr.sh_offset);
        }
        if (strcmp(&section_names[shdr.sh_name], ".signature") == 0) {
            //spi_mem_read(spi_mem, flash_addr+shdr.sh_offset, &config.signature, shdr.sh_size);
        }
        offset += sizeof(shdr);
    }
    image_boot_args.entry = (void*)ehdr.e_entry;

    return SSBL_SUCCESS;
}

int
ssbl_boot_index_get(void)
{
    FILE *f_part;
    struct json_parser json;
    int ch='A';
    int  boot_index = -1;
    unsigned int state = JSON_PARSE_STATE_KEY;

    /*open part.json and parse the file*/
    f_part = fopen(BOOT_FILE_PATH, "r");
    if(NULL== f_part){
        os_printf("\nCritical Error: Could not open /root/boot.json file");
        return -1;
    }
    
    json_init(&json);

    while(1){       
        ch = fgetc(f_part);
        if(ch == EOF){
            break;
        }
        
        volatile __auto_type t = json_tokenizer(&json, ch);
        if(t == JSON_END)
            break;
        if(t == JSON_BEGIN_ARRAY) {
            state = JSON_PARSE_STATE_KEY;
            continue;
        }

        if(JSON_PARSE_STATE_KEY == state && t == JSON_STRING) {
            ssbl_printf("\nkey =  %s\n",json.token);
            state =JSON_PARSE_STATE_SEPRATOR;
            if (strcmp(json.token, "boot_index") == 0) {
                /*index_found = 1;*/
            } 
            continue;
        }

        if(JSON_PARSE_STATE_SEPRATOR == state && t == JSON_MEMBER_SEPARATOR) {
            state = JSON_PARSE_STATE_VAL;
            continue;
        }

        if(state == JSON_PARSE_STATE_VAL && t == JSON_NUMBER) {
            ssbl_printf("num val : %d\n",atoi(json.token));                
            boot_index = atoi(json.token);
            /*got the boot index*/
            state = JSON_PARSE_STATE_KEY;
            break;
        }
    }

    json_finish(&json);
    fclose(f_part);
    return boot_index;
}

void 
ssbl_add_boot_arg(char* arg, size_t len)
{
    *--boot_arg_p = 0;
    boot_arg_p -= len;
    image_boot_args.argc++;
    memcpy(boot_arg_p, arg, len);
}

int
ssbl_parse_part_file(void)
{
    FILE *f_part;
    struct json_parser json;
    int ch='A';
    void *dest =NULL;
    int  image_index = 0;
    unsigned int state = JSON_PARSE_STATE_KEY;
    int boot_arg = 0;
    part_image_info_t * image_info = NULL;
    char *boot_arg_key;

    memset(&part_file, 0, sizeof(part_file));
    
    if(NULL == (boot_arg_key = os_alloc(256))){
        os_printf("\nError: malloc at %d", __LINE__);
        return SSBL_ERROR;
    }
    
    ssbl_printf("\nNote: Max size of json token = %d", sizeof(json.token));

    /*open part.json and parse the file*/
    f_part = fopen(PART_FILE_PATH, "r");
    if(NULL== f_part){
        os_printf("\nCritical Error: Could not open /root/part.json file");
        return SSBL_ERROR;
    }
    json_init(&json);
    boot_arg_key[0] = '\0';
    while(1){
        if(ch == EOF){
            ssbl_printf("\nEOF reached");
            break;
        }
        ch = fgetc(f_part);
        volatile __auto_type t = json_tokenizer(&json, ch);
        if(t == JSON_END)
            break;
        if(t == JSON_BEGIN_ARRAY) {
            state = JSON_PARSE_STATE_KEY;
            continue;
        }

        if(JSON_PARSE_STATE_KEY == state && t == JSON_STRING) {
            ssbl_printf("\nKey= %s",json.token);
            state =JSON_PARSE_STATE_SEPRATOR;
            dest = NULL;
            if (strcmp(json.token, "name") == 0) {
                image_info = os_alloc(sizeof(part_image_info_t));
                if(NULL == image_info){
                    ssbl_printf("\nError: malloc at %d", __LINE__);
                    os_free(boot_arg_key);
                    return SSBL_ERROR;
                }
                /*Insert at the head*/
                image_info->next = part_file.image_info_list;
                part_file.image_info_list = image_info;
                image_info->index = image_index;
                image_index++;
                boot_arg = 0;
                image_info->boot_args_cnt = 0;
                dest = image_info->name;
                
            }
            if(image_info){
                if (strcmp(json.token, "version") == 0) {
                    dest = image_info->version;
                } else if (strcmp(json.token, "start_sector") == 0) {
                    dest = &image_info->sector;
                }else if (strcmp(json.token, "bootargs_start") == 0) {
                    ssbl_printf("\nbootargs_start");
                    boot_arg = 1;
                }else if (strcmp(json.token, "bootargs_end") == 0) {
                    ssbl_printf("\nbootargs_end");
                    boot_arg = 0;
                }else if(boot_arg){/*This need to be at the end of this 
                 els-if chain*/                    
                    strcpy(boot_arg_key, json.token);                   
                }
            }
            continue;
        }

        if(JSON_PARSE_STATE_SEPRATOR == state && t == JSON_MEMBER_SEPARATOR) {
            state = JSON_PARSE_STATE_VAL;
            continue;
        }

        if(state == JSON_PARSE_STATE_VAL) {
            if(t == JSON_STRING) {
                ssbl_printf("\nVal(str)= %s",json.token);
                if(boot_arg && image_info){
                    int len = strlen(boot_arg_key) + strlen("=") +
                                                     strlen(json.token) + 1;
                    char *p = os_alloc(len);
                    *p = '\0'; 
                    strcat(p, boot_arg_key);
                    strcat(p, "=");
                    strcat(p, json.token);
                    image_info->boot_args[image_info->boot_args_cnt] = p;
                    image_info->boot_args_cnt++;
                }else if(dest != NULL)
                    strcpy(dest, json.token);                                
            } else if(t == JSON_NUMBER) {
                ssbl_printf("\nVal(NUM)= %d",atoi(json.token));
                if(dest != NULL)
                    *(int*)dest = atoi(json.token);
            }
            if(t == JSON_STRING || t == JSON_NUMBER){ 
                state = JSON_PARSE_STATE_KEY;
                dest =NULL;
            }
        }
    }

    json_finish(&json);
    fclose(f_part);
    return SSBL_SUCCESS;
    
}

part_image_info_t *
ssbl_image_info_get(int boot_index)
{
    part_image_info_t *p;
    int i;
    /*get the version of the current image @boot_index */
    p = part_file.image_info_list;
    while(p){
        if(boot_index == p->index)
            break;
        p = p->next;
    }
    if(NULL == p){
        return p;
    }

    /*Add boot args*/
    for(i = 0; i < p->boot_args_cnt; i++){
        char *arg =p->boot_args[i];
        ssbl_add_boot_arg(arg, strlen(arg));
    }
    return p;
}

int ssbl_file_integrity_ckeck(const char *file_path,
                              const char *chksum_file_path,
                              char *chksum_out)
{
    FILE *f;
    unsigned int f_chksum;
    char chksum_str[16];
    char buff[16];

    chksum_out[0] = '\0';
    ssbl_printf("\nIntegrity Check : %s", file_path);
    fletcher32_file_checksum_get(file_path, &f_chksum);
    sprintf(chksum_str, "%08x", f_chksum);

    f = fopen(chksum_file_path, "r");
    if(NULL == f){
        return -1;
    }
    fread(buff, 1, 8, f);
    buff[8] = '\0';

    ssbl_printf("\nchecksum (calculated) = %s", chksum_str);
    ssbl_printf("\nchecksum (stored)     = %s", buff);

    strcpy(chksum_out, chksum_str);
    return !strcmp(chksum_str, buff);
    
}


/*Restore specified file from the backup*/
void ssbl_file_restore(const char *file_path, const char *chksum_file_path,
                       const char *bckup_file_path, 
                       const char *bckup_chksum_file_path)
{
    utils_file_clone(file_path, bckup_file_path);
    utils_file_clone(chksum_file_path, bckup_chksum_file_path);        
}

void ssbl_fota_cfg_file_restore(void)
{
    ssbl_file_restore(FOTA_CFG_FILE_PATH, FOTA_CFG_CHKSUM_FILE_PATH,
                      FOTA_CFG_BACKUP_FILE_PATH,
                      FOTA_CFG_BACKUP_CHKSUM_FILE_PATH);
}
void ssbl_part_file_restore(void)
{
    ssbl_file_restore(PART_FILE_PATH, PART_CHKSUM_FILE_PATH,
                      PART_BACKUP_FILE_PATH, PART_BACKUP_CHKSUM_FILE_PATH);
}

int ssbl_file_sanity_check(const char *file_path, const char *chksum_file_path,
                                const char *bckup_file_path, 
                                const char *bckup_chksum_file_path)
{
    int rval, rval_bkp;
    char chksum[16], chksum_bkp[16];
    int take_backup = 0;

    /*check integrity of file*/
    rval = ssbl_file_integrity_ckeck(file_path, chksum_file_path, chksum);

    rval_bkp = ssbl_file_integrity_ckeck(bckup_file_path, bckup_chksum_file_path,
                                         chksum_bkp);
    if(rval && rval_bkp){
        if(strcmp(chksum, chksum_bkp)){
            take_backup = 1;            
            ssbl_printf("\n%s : Master copy is intact. Backup copy Not present", 
                        file_path);
        }else{            
            ssbl_printf("\n%s : Both the files are intact: %s : %s", 
                        file_path, chksum, chksum_bkp);
        }
    }
    else if(!rval && !rval_bkp){
        os_printf("\nCritical error: Sanity check fail for file %s : "
                  "Both the files are NOT!! intact\n", file_path);
        os_park_thread(os_self());
    }
    if(0 == rval){
        ssbl_printf("\n%s : Is is not intact", file_path);
        utils_file_clone(file_path, bckup_file_path);
        utils_file_clone(chksum_file_path, bckup_chksum_file_path);
        return SSB_SANITY_ERROR_MASTER_COPY;
    }
    
    if(0 == rval_bkp || take_backup){
        ssbl_printf("\n%s : Is is not intact or take_backup = %d", 
                    bckup_file_path, take_backup);
        utils_file_clone(bckup_file_path, file_path);
        utils_file_clone(bckup_chksum_file_path, chksum_file_path);
        return take_backup?SSB_SANITY_ERROR_NONE: SSB_SANITY_ERROR_BACKUP_COPY;
    }

    return SSB_SANITY_ERROR_NONE;
}

void ssbl_sanity_check(void)
{
    int fota_in_progress;
    int rval;
    if(!utils_is_file_present(DIRTY_BIT_FILE_PATH)){
        ssbl_printf("\nDirty bit file not present. All ok");
        return;
    }
    fota_in_progress = utils_is_file_present(DIRTY_BIT_FILE_PATH);
    ssbl_printf("\n\n**Checking Sanity of : %s **\n\n", FOTA_CFG_FILE_PATH);
    rval = ssbl_file_sanity_check(FOTA_CFG_FILE_PATH, 
                                  FOTA_CFG_CHKSUM_FILE_PATH,
                                  FOTA_CFG_BACKUP_FILE_PATH,
                                  FOTA_CFG_BACKUP_CHKSUM_FILE_PATH);
    if(fota_in_progress&& rval == SSB_SANITY_ERROR_MASTER_COPY){
        /*Nothing to roll back*/
        goto exit_error;        
    }
    ssbl_printf("\n\n**Checking Sanity of : %s **\n\n", PART_FILE_PATH);
    rval = ssbl_file_sanity_check(PART_FILE_PATH, PART_CHKSUM_FILE_PATH,
                                  PART_BACKUP_FILE_PATH,
                                  PART_BACKUP_CHKSUM_FILE_PATH);    
    if(fota_in_progress&& rval == SSB_SANITY_ERROR_MASTER_COPY){
        /*Roll back fota_config.json file*/
        ssbl_fota_cfg_file_restore();
        goto exit_error;
        
    }        
    ssbl_printf("\n\n**Checking Sanity of : %s **\n\n", BOOT_FILE_PATH);
    rval = ssbl_file_sanity_check(BOOT_FILE_PATH, BOOT_CHKSUM_FILE_PATH,
                                  BOOT_BACKUP_FILE_PATH,
                                  BOOT_BACKUP_CHKSUM_FILE_PATH);    
    if(fota_in_progress&& rval == SSB_SANITY_ERROR_MASTER_COPY){
        /*Roll back fota_config.json and part.json file*/
        ssbl_fota_cfg_file_restore();
        ssbl_part_file_restore();
        goto exit_error;
        
    }
    
    unlink(DIRTY_BIT_FILE_PATH);
    unlink(FOTA_IN_PROGRESS_FILE_PATH);
    ssbl_printf("\nSanity check OK.. removed dirty bit file");
    return;
exit_error:
    unlink(FOTA_IN_PROGRESS_FILE_PATH);
    /*Reboot the device*/
    reset_device();
}

int main(void)
{
    part_image_info_t *image_info;
    int boot_index, rval;
    
    print_ver("\nSSBL Application", 1, 1);

    rval = utils_mount_rootfs();
    if(0 != rval)
    {
        os_printf("\nCritical Error: Mounting rootfs\n");
        os_park_thread(os_self());
    }

    /*Sanity check on control and config files*/
    ssbl_sanity_check();

    /*parse part file*/
    rval = ssbl_parse_part_file();
    if(rval != SSBL_SUCCESS){        
        os_printf("\nCritical Error: Failed Parsing part file.\n");
        os_park_thread(os_self());
    }
    /*Get the boot index*/
    boot_index = ssbl_boot_index_get(); 
    os_printf("\nBoot indx = %d", boot_index);
    if(boot_index < 0){
        os_printf("\nCritical Error: Improper Boot indx = %d\n", boot_index);
        os_park_thread(os_self());
    }
    /*Get the image info for the boot_index*/
    image_info = ssbl_image_info_get(boot_index);
    if(NULL == image_info){
        os_printf("\nCritical Error: No image info found, bootindex = %d\n",
                    boot_index);
        os_park_thread(os_self());
    }
    /*Load the selected image*/
    if (ssbl_load_elf(image_info->sector * SSBL_FLASH_SCTOR_SIZE) < 0) {
        os_printf("\nCritical Error : Illegal elf @bootindex = %d\n", boot_index);
        /*TODO: can the logic here is to load factory image or FOTA?*/
        os_park_thread(os_self());
    }
    os_printf("\nElf Load OK...\n");
    
    if (vm_flash_location) {
        boot_arg_p -= 29;
        image_boot_args.argc++;
        ssbl_printf("vm_flash location: %d\n", vm_flash_location);
        snprintf(boot_arg_p, 29, "vm.flash_location=0x%08x", vm_flash_location);
    }
    /*Start the loaded in to RAM. This shall never return*/
    ssbl_start_image();

    return -1;
}




