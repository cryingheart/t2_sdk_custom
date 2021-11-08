#include <assert.h>

#include <kernel/debug.h>
#include <kernel/os.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

#define OK 0
#define ERROR -1

#define CONFIG_DEBUG_FEATURES              1
#define CONFIG_DISABLE_MQUEUE              1
#define CONFIG_FS_FAT                      1
#define CONFIG_FS_READABLE                 1
#define CONFIG_FS_WRITABLE                 1
#define CONFIG_NFILE_STREAMS               8
#define CONFIG_NFILE_DESCRIPTORS           1
#define CONFIG_PATH_MAX                    128
#define CONFIG_FS_TMPFS                    1
#define CONFIG_FS_TMPFS_BLOCKSIZE          512
#define CONFIG_FS_TMPFS_DIRECTORY_ALLOCGUARD 64
#define CONFIG_FS_TMPFS_DIRECTORY_FREEGUARD  128
#define CONFIG_FS_TMPFS_FILE_ALLOCGUARD      64
#define CONFIG_FS_TMPFS_FILE_FREEGUARD       128
#define CONFIG_STDIO_DISABLE_BUFFERING 1
#define CONFIG_MTD                         1
#define CONFIG_FS_LITTLEFS                 1

#define DEBUGASSERT(x) assert(x)
#define ASSERT(x)      assert(x)

#define getpid() ((int)os_self())

#define weak_function

#define O_RDOK      O_RDONLY
#define O_WROK      O_WRONLY

/* belongs somewhere else */
#define ferr(format, ...)     pr_debug(format, ##__VA_ARGS__)
#define fwarn(format, ...)    pr_debug(format, ##__VA_ARGS__)
#define finfo(format, ...)    pr_debug(format, ##__VA_ARGS__)
#define _err ferr

/* possibly belongs in limits.h */
#define NAME_MAX 32 /* max filename (component) length */
#define PATH_MAX 64 /* max pathname length */
