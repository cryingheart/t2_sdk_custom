//#define OS_DEBUG PR_LEVEL_DEBUG
#include <errno.h>
#include <kernel/os.h>
#include <kernel/hostio.h>
#include <sys/statfs.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include "storage.h"

/*
 * HOST INTERFACE FUNCTIONS
 */

static int file_size(const char* path)
{
    int result;

    FILE *fp = fopen(path, "rb");
    if (!fp)
        return -EINVAL;
    fseek(fp, 0, SEEK_END);
    result = ftell(fp);
    fclose(fp);
    return result;
}

static char* alloc_path(const char *path, const char *fn)
{
    char *result = os_alloc(2 + strlen(path) + strlen(fn));
    strcpy(result, path);
    strcat(result, "/");
    strcat(result, fn);
    return result;
}


/* return size of data in chain starting with data; ptr is set to
 * linear block of memory containing all data, either just a pointer
 * to memory in fragment if already linear, or allocated memory
 */
size_t linearise_pfrag_chain(struct pfrag *data, void **ptr)
{
    size_t size = 0;
    uint8_t *p;
    struct pfrag *pf;

    /* data should to be NULL or non-zero at this point */
    assert(data == NULL || pfrag_size(data) > 0);

    for(pf = data; pf != NULL; pf = pfrag_next(pf))
        size += pfrag_size(pf);

    if(size == 0) {
        *ptr = NULL;
        return 0;
    }

    if(size == pfrag_size(data)) {
        *ptr = pfrag_head(data);
        return size;
    }
    p = *ptr = os_alloc(size);
    if(ptr == NULL)
        return size;

    for(pf = data; pf != NULL; pf = pfrag_next(pf)) {
        memcpy(p, pfrag_head(pf), pfrag_size(pf));
        p += pfrag_size(pf);
    }
    return size;
}


int read_file(const char *path, char* buf, size_t length, size_t offset)
{
    FILE *fp = fopen(path, "rb");
    int result;

    if (!fp)
        return -EINVAL;

    if (fseek(fp, offset, SEEK_SET) != 0)
        return -errno;

    result = fread(buf, 1, length, fp);

    fclose(fp);
    return result;
}


static struct pfrag*
handle_file_read(struct storage_file_read_req *req,
                 struct storage_file_read_rsp *rsp)
{
    struct pfrag *pf = pfrag_alloc(req->length);
    int result;

    if (pf == NULL) {
        rsp->status = ENOMEM;
        return NULL;
    }

    pr_debug("%s path:%s off:%d len:%d\n", __func__, (char*)req->path, req->offset, req->length);

    result = read_file((char*)req->path, pfrag_head(pf), req->length, req->offset);

    pr_debug("%s result:%d\n", __func__, result);
#if (OS_DEBUG == PR_LEVEL_DEBUG)
    if (result > 0)
        os_console_hexdump("READ", pfrag_head(pf), result);
#endif

    if (result >= 0) {
        rsp->status = 0;
        pfrag_insert_tail(pf, result);
        return pf;
    } else {
        rsp->status = -result;
        pfrag_free(pf);
        return NULL;
    }
}


int write_file(const char *path, size_t offset, struct pfrag *data)
{
    FILE *fp;
    struct pfrag *pf;
    int result = 0;

    if (offset == 0)
        fp = fopen(path, "wb");
    else
    {
        fp = fopen(path, "ab");
        fseek(fp, 0, SEEK_END);
        result = ftell(fp);
        if (result != offset) {
            pr_always("%s: %d != %d\n", __func__, result, offset);
            fclose(fp);
            return -EIO;
        }
    }

    if (!fp)
        return -errno;

    for(pf = data; pf != NULL; pf = pfrag_next(pf))
    {
#if (OS_DEBUG == PR_LEVEL_DEBUG)
        os_console_hexdump("WRITE", pfrag_head(pf), pfrag_size(pf));
#endif
        int n = fwrite( pfrag_head(pf), 1, pfrag_size(pf), fp);
        result += n;
        if (n != pfrag_size(pf))
            break;
    }

    fclose(fp);

    return result;
}


void
handle_file_write(struct storage_file_write_req *req,
                  struct pfrag *data,
                  struct storage_file_write_rsp *rsp)
{
    int result;
    int length = 0;

    for(struct pfrag *pf = data; pf != NULL; pf = pfrag_next(pf))
        length += pfrag_size(pf);

    result = write_file((char*)&req->path[0], req->offset, data);

    pr_info("%s: mode:%d off:%d path:%s %d\n", __func__,
              req->mode, req->offset, req->path, result);

    if (result < 0) {
        rsp->status = result;
        rsp->length = 0;
        return;
    }

    rsp->length = result;
    if (result == length)
        rsp->status = 0;
    else
        rsp->status = EIO;
}

void handle_file_unlink(struct storage_file_unlink_req *req,
                        struct storage_file_unlink_rsp *rsp)
{
    rsp->status = 0;
    if (0 != unlink(req->path))
        rsp->status = -errno;
}


void handle_file_mkdir(struct storage_file_mkdir_req *req,
                       struct storage_file_mkdir_rsp *rsp)
{
    rsp->status = 0;
    if (0 != mkdir(req->path, 0))
        rsp->status = -errno;
}


void handle_file_rmdir(struct storage_file_rmdir_req *req,
                       struct storage_file_rmdir_rsp *rsp)
{
    rsp->status = 0;
    if (0 != rmdir(req->path))
        rsp->status = -errno;
}


void handle_file_rename(struct storage_file_rename_req *req,
                        struct storage_file_rename_rsp *rsp)
{
    rsp->status = 0;
    if (0 != rename(req->oldpath, req->newpath))
        rsp->status = -errno;
}


static int fill_stat(const char *path, uint32_t *size, uint32_t *mode)
{
    struct stat st;
    if (stat(path, &st) != 0) {
        *mode = 0;
        *size = 0;
        return -errno;
    }
    *mode = st.st_mode;
    *size = st.st_size;
    return 0;
}


void handle_file_stat(struct storage_file_stat_req *req,
                      struct storage_file_stat_rsp *rsp)
{
    rsp->status = fill_stat(req->path, &rsp->size, &rsp->mode);
}


static struct dirent*
get_dirent_at_offset(DIR *dp, unsigned int offset)
{
    struct dirent *dent;
    for (int i=0; true ; i++)
    {
        dent = readdir(dp);
        if (!dent) {
            return NULL;
        }

        if (offset != i)
            continue;

        return dent;
    }
    return NULL;
}


struct pfrag*
handle_file_dir_read(struct storage_file_dir_read_req *req,
                     struct storage_file_dir_read_rsp *rsp)
{
    DIR *dp = opendir((char*)&req->path[0]);
    struct dirent *dent;

    rsp->mode = 0;
    rsp->size = 0;

    if (!dp) {
        rsp->status = -EINVAL;
        return NULL;
    }

    dent = get_dirent_at_offset(dp, req->offset);
    if(!dent) {
        closedir(dp);
        rsp->status = -EIO;
        return NULL;
    }

    uint32_t actual = strlen(dent->d_name);
    struct pfrag *pf = pfrag_alloc(actual);
    memcpy(pfrag_insert_tail(pf, actual), dent->d_name, actual);
    rsp->mode = dent->d_type;
    rsp->status = 0;

#if 1
    if (rsp->mode == DTYPE_FILE) // file
    {
        char *fn = alloc_path(req->path, dent->d_name);
        //rsp->status = fill_stat(fn, &rsp->size, &rsp->mode);
        rsp->size = file_size(fn);
        os_free(fn);
    }
#endif

    closedir(dp);

    return pf;
}

//[[[cog
//import generate
//by_name = dict()
//for cls in generate.registry.messages('storage'):
//    by_name[cls.__name__] = cls
//def make_wrapper(name):
//    cls = by_name[name]
//    req = cls.get_fields('req')
//    variable_req = req and req[-1].array == 0
//    rsp = cls.get_fields('rsp')
//    variable_rsp = rsp and rsp[-1].array == 0
//
//    cog.outl(f'static struct packet*')
//    cog.outl(f'storage_{name}(void *ctx, struct packet *msg)')
//    cog.outl('{')
//    cog.outl(f'    struct storage_{name}_req *req;')
//    cog.outl(f'    struct storage_{name}_rsp *rsp;')
//    cog.outl(f'    struct packet *pkt = alloc_storage_{name}_rsp(&rsp);')
//    cog.outl(f'    struct pfrag *pf;')
//    cog.outl(f'')
//    cog.outl(f'    OS_ERROR_ON_NULL(pkt);')
//    cog.outl(f'')
//    cog.outl(f'    pf = packet_first_frag(msg);')
//    cog.outl('    if (pfrag_size(pf) < sizeof(*req)) {')
//    cog.outl(f'        rsp->status = EIO;')
//    cog.outl(f'        return pkt;')
//    cog.outl('    }')
//    cog.outl(f'')
//    cog.outl(f'    req = pfrag_head(pf);')
//    args = ['req', 'rsp']
//    if variable_req:
//        cog.outl(f'    pfrag_remove_head(pf, sizeof(*req));')
//        cog.outl(f'    while(pf != NULL && pfrag_size(pf) == 0)')
//        cog.outl(f'        pf = pfrag_next(pf);')
//        cog.outl(f'')
//        args.insert(1, 'pf')
//    args = ', '.join(args)
//    if variable_rsp:
//        cog.outl(f'    pf = handle_{name}({args});')
//        cog.outl(f'    if (pf != NULL)')
//        cog.outl(f'        packet_add_frag(pkt, pf);')
//    else:
//        cog.outl(f'    handle_{name}({args});')
//    cog.outl(f'')
//    cog.outl(f'    return pkt;')
//    cog.outl('}')
//    return
//
//typenames = '''
//file_read
//file_write
//file_unlink
//file_mkdir
//file_rmdir
//file_rename
//file_stat
//file_dir_read
//'''.split()
//
//for i, name in enumerate(typenames):
//    if i > 0:
//        cog.outl('')
//    make_wrapper(name)
//]]]
static struct packet*
storage_file_read(void *ctx, struct packet *msg)
{
    struct storage_file_read_req *req;
    struct storage_file_read_rsp *rsp;
    struct packet *pkt = alloc_storage_file_read_rsp(&rsp);
    struct pfrag *pf;

    OS_ERROR_ON_NULL(pkt);

    pf = packet_first_frag(msg);
    if (pfrag_size(pf) < sizeof(*req)) {
        rsp->status = EIO;
        return pkt;
    }

    req = pfrag_head(pf);
    pf = handle_file_read(req, rsp);
    if (pf != NULL)
        packet_add_frag(pkt, pf);

    return pkt;
}

static struct packet*
storage_file_write(void *ctx, struct packet *msg)
{
    struct storage_file_write_req *req;
    struct storage_file_write_rsp *rsp;
    struct packet *pkt = alloc_storage_file_write_rsp(&rsp);
    struct pfrag *pf;

    OS_ERROR_ON_NULL(pkt);

    pf = packet_first_frag(msg);
    if (pfrag_size(pf) < sizeof(*req)) {
        rsp->status = EIO;
        return pkt;
    }

    req = pfrag_head(pf);
    pfrag_remove_head(pf, sizeof(*req));
    while(pf != NULL && pfrag_size(pf) == 0)
        pf = pfrag_next(pf);

    handle_file_write(req, pf, rsp);

    return pkt;
}

static struct packet*
storage_file_unlink(void *ctx, struct packet *msg)
{
    struct storage_file_unlink_req *req;
    struct storage_file_unlink_rsp *rsp;
    struct packet *pkt = alloc_storage_file_unlink_rsp(&rsp);
    struct pfrag *pf;

    OS_ERROR_ON_NULL(pkt);

    pf = packet_first_frag(msg);
    if (pfrag_size(pf) < sizeof(*req)) {
        rsp->status = EIO;
        return pkt;
    }

    req = pfrag_head(pf);
    handle_file_unlink(req, rsp);

    return pkt;
}

static struct packet*
storage_file_mkdir(void *ctx, struct packet *msg)
{
    struct storage_file_mkdir_req *req;
    struct storage_file_mkdir_rsp *rsp;
    struct packet *pkt = alloc_storage_file_mkdir_rsp(&rsp);
    struct pfrag *pf;

    OS_ERROR_ON_NULL(pkt);

    pf = packet_first_frag(msg);
    if (pfrag_size(pf) < sizeof(*req)) {
        rsp->status = EIO;
        return pkt;
    }

    req = pfrag_head(pf);
    handle_file_mkdir(req, rsp);

    return pkt;
}

static struct packet*
storage_file_rmdir(void *ctx, struct packet *msg)
{
    struct storage_file_rmdir_req *req;
    struct storage_file_rmdir_rsp *rsp;
    struct packet *pkt = alloc_storage_file_rmdir_rsp(&rsp);
    struct pfrag *pf;

    OS_ERROR_ON_NULL(pkt);

    pf = packet_first_frag(msg);
    if (pfrag_size(pf) < sizeof(*req)) {
        rsp->status = EIO;
        return pkt;
    }

    req = pfrag_head(pf);
    handle_file_rmdir(req, rsp);

    return pkt;
}

static struct packet*
storage_file_rename(void *ctx, struct packet *msg)
{
    struct storage_file_rename_req *req;
    struct storage_file_rename_rsp *rsp;
    struct packet *pkt = alloc_storage_file_rename_rsp(&rsp);
    struct pfrag *pf;

    OS_ERROR_ON_NULL(pkt);

    pf = packet_first_frag(msg);
    if (pfrag_size(pf) < sizeof(*req)) {
        rsp->status = EIO;
        return pkt;
    }

    req = pfrag_head(pf);
    handle_file_rename(req, rsp);

    return pkt;
}

static struct packet*
storage_file_stat(void *ctx, struct packet *msg)
{
    struct storage_file_stat_req *req;
    struct storage_file_stat_rsp *rsp;
    struct packet *pkt = alloc_storage_file_stat_rsp(&rsp);
    struct pfrag *pf;

    OS_ERROR_ON_NULL(pkt);

    pf = packet_first_frag(msg);
    if (pfrag_size(pf) < sizeof(*req)) {
        rsp->status = EIO;
        return pkt;
    }

    req = pfrag_head(pf);
    handle_file_stat(req, rsp);

    return pkt;
}

static struct packet*
storage_file_dir_read(void *ctx, struct packet *msg)
{
    struct storage_file_dir_read_req *req;
    struct storage_file_dir_read_rsp *rsp;
    struct packet *pkt = alloc_storage_file_dir_read_rsp(&rsp);
    struct pfrag *pf;

    OS_ERROR_ON_NULL(pkt);

    pf = packet_first_frag(msg);
    if (pfrag_size(pf) < sizeof(*req)) {
        rsp->status = EIO;
        return pkt;
    }

    req = pfrag_head(pf);
    pf = handle_file_dir_read(req, rsp);
    if (pf != NULL)
        packet_add_frag(pkt, pf);

    return pkt;
}
//[[[end]]]

/*
[[[cog
import cog, generate
generate.api(generate.Emitter(cog.out), 'storage')
]]] */
static const struct hio_api storage_api = {
    .group = 14,
    .num_handlers = 8,
    .handler = {
        storage_file_read,
        storage_file_write,
        storage_file_unlink,
        storage_file_mkdir,
        storage_file_rmdir,
        storage_file_rename,
        storage_file_stat,
        storage_file_dir_read,
    }
};
/* [[[end]]] */

void
storage_hio_init(void)
{
    hio_api_init(&storage_api, NULL);
}
