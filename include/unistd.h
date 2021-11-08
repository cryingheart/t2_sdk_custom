#pragma once

#include <bits/size_t.h>
#include <bits/ssize_t.h>
#include <bits/off_t.h>
#include <bits/seek.h>

struct file;

int          file_close(struct file *filep);
off_t        file_lseek(struct file *filep, off_t offset, int whence);
ssize_t      file_read(struct file *filep, void *buf, size_t nbytes);
ssize_t      file_write(struct file *filep, const void *buf, size_t nbytes);
int          file_ftruncate(struct file *filep, off_t length);
int          file_fsync(struct file *filep);
int          unlink(const char *pathname);
int          rmdir(const char *pathname);
