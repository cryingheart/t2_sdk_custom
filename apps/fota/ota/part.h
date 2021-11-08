#pragma once

struct image_config
{
    char name[32];
    char version[16];
    uint8_t status; //ToDo: Use to determine if image is valid, invalid, mark for delete, etc
    uint32_t sector;
    uint32_t size;
    uint32_t sector_count;
    char ecdsa_pub[64];
    char *args[32];
};

struct part_config 
{
    int baud;
    int timeout;
    char common[256];
    int image_count;
    int rollback_image;
    int verbose;
    int insecure;
    int fail_check;
    struct image_config image_config[8];
};

int next_free_sector(struct part_config* part_config);

int avail_sectors(struct part_config* part_config);