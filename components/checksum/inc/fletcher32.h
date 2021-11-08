#pragma once

unsigned int fletcher32(const unsigned short *data, size_t len);

/**
 * Return -1 if failure. The checksum is returned through checksum_out
 */
int 
fletcher32_file_checksum_get(const char *path, unsigned int *checksum_out);

