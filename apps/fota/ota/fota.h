#pragma once

struct ota_config {
    char ssid[32];
    char passphrase[32];
    uint8_t ota_flag;
    char server[128];
    char server_port[6];
    char name[32];
    char version[16];
    char filename[32];
    uint32_t filesize;
    char username[32];
    char password[32];
    char hash[64];
    char args[128];
    uint8_t param_count;
};