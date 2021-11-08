#pragma once
#include "fs_utils.h"

//#define UTILS_DEBUG_ENABLE

#ifdef UTILS_DEBUG_ENABLE
#define utils_printf(_fmt, ...)   os_printf(_fmt, ##__VA_ARGS__)
#else
#define utils_printf(_fmt, ...)
#endif

void print_app_info(char *name, char *version);
void reset_device(void);
void show_heap(char *function, char *line_num);

void DumpHex(char* description, const void* data, size_t size, int show_string);

void print_ver(char *banner, int print_sdk_name, int print_emb_app_ver);

int utils_num_str_cmp(const char *str1, const char *str2);

char* reverse_str(char *s);

#define SHOW_HEAP_AVAILABLE()     show_heap(__FUNCTION__, __LINE__)
