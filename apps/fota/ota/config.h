#pragma once

#define CIPHER_KEY_LEN 32
#define ECDSA_PUBLIC_KEY_LEN 64

struct config {
    uint32_t baudrate;
    uint32_t timeout;
    uint32_t verbose;
    uint32_t insecure;
    uint32_t fail_check;
    uint32_t image_start_sector;
    uint32_t image_size;
    uint32_t image_sector_count;
    char     image_name[40];
    char     image_version[16];
    uint8_t  cipher_key[CIPHER_KEY_LEN];
    uint8_t  ecdsa_public_key[ECDSA_PUBLIC_KEY_LEN];
    uint8_t  signature[64];

};


extern struct config config;
