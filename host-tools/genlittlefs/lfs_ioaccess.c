// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <lfs.h>
#include "lfs_ioaccess.h"
#include "parse_partitions.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <linux/fs.h>
#include <sys/ioctl.h>

struct lfs_ioaccess_context
{
    int file_des;
    loff_t part_offs;
    size_t last_offs;
    const void *empty_flash_mem;
    struct timeval last_sync;
};

static int lfs_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    struct lfs_ioaccess_context *ctx = c->context;
    if (!ctx) {
        return LFS_ERR_IO;
    }
    const off_t offrq = (off_t)block * c->block_size + (off_t)off + ctx->part_offs;
    if (offrq > (off_t)ctx->last_offs + size) {
        return LFS_ERR_IO;
    }
    off_t err = lseek(ctx->file_des, offrq, SEEK_SET);
    if (err < 0) {
        return -errno;
    }
    else if (err != offrq) {
        return LFS_ERR_IO;
    }
    char *rd_buf = buffer;
    do {
        ssize_t ret = read(ctx->file_des, rd_buf, (size_t)size);
        if (ret > 0) {
            size -= ret;
            rd_buf += ret;
        }
        else if (ret == 0) {
            break;
        }
        else {
            return ret;
        }
    } while (size > 0);
    return size > 0 ? LFS_ERR_IO : 0;
}

static int lfs_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    struct lfs_ioaccess_context *ctx = c->context;
    if (!ctx) {
        return LFS_ERR_IO;
    }
    const off_t offrq = (off_t)block * c->block_size + (off_t)off + ctx->part_offs;
    if (offrq > (off_t)ctx->last_offs + size) {
        return LFS_ERR_IO;
    }
    off_t err = lseek(ctx->file_des, offrq, SEEK_SET);
    if (err < 0) {
        return -errno;
    }
    else if (err != offrq) {
        return LFS_ERR_IO;
    }
    const char *wr_buf = buffer;
    do {
        ssize_t ret = write(ctx->file_des, wr_buf, (size_t)size);
        if (ret > 0) {
            size -= ret;
            wr_buf += ret;
        }
        else if (ret == 0) {
            break;
        }
        else {
            return ret;
        }
    } while (size > 0);
    return size > 0 ? LFS_ERR_IO : 0;
}

static int lfs_erase(const struct lfs_config *c, lfs_block_t block)
{
    struct lfs_ioaccess_context *ctx = c->context;
    if (!ctx) {
        return LFS_ERR_IO;
    }
    const off_t offrq = (off_t)block * c->block_size + ctx->part_offs;
    if (offrq > (off_t)ctx->last_offs + c->block_size) {
        return LFS_ERR_IO;
    }
    off_t err = lseek(ctx->file_des, offrq, SEEK_SET);
    if (err < 0) {
        return -errno;
    }
    else if (err != offrq) {
        return LFS_ERR_IO;
    }
    const char *wr_buf = ctx->empty_flash_mem;
    size_t size;
    do {
        size        = c->block_size;
        ssize_t ret = write(ctx->file_des, wr_buf, (size_t)size);
        if (ret > 0) {
            size -= ret;
            wr_buf += ret;
        }
        else if (ret == 0) {
            break;
        }
        else {
            return ret;
        }
    } while (size > 0);
    return size > 0 ? LFS_ERR_IO : 0;
}

static int lfs_sync(const struct lfs_config *c)
{
    struct lfs_ioaccess_context *ctx = c->context;
    struct timeval curr_msync, result_msync;
    if (gettimeofday(&curr_msync, NULL) == -1) {
        return -1;
    }
    timersub(&curr_msync, &ctx->last_sync, &result_msync);
    int err = 0;
    if (result_msync.tv_sec >= 1) {
        err            = fsync(ctx->file_des);
        ctx->last_sync = curr_msync;
    }
    return err;
}

#ifdef LFS_THREADSAFE
//! Note single threaded application don't need locks
static int lfs_lock_unlock(const struct lfs_config *cfg)
{
    (void)cfg;
    return LFS_ERR_OK;
}
#endif

struct lfs_ioaccess_context *lfs_ioaccess_open(struct lfs_config *cfg,
                                               const char *filename,
                                               const struct partition *partition)
{
    struct lfs_ioaccess_context *ret = calloc(1, sizeof(struct lfs_ioaccess_context));
    if (!ret) {
        return NULL;
    }
    {
        char *memm = malloc(cfg->block_size);
        if (!memm) {
            free(ret);
            return NULL;
        }
        memset(memm, 0xff, cfg->block_size);
        ret->empty_flash_mem = memm;
    }
    ret->file_des = open(filename, O_RDWR);
    if (ret->file_des < 0) {
        free((void *)ret->empty_flash_mem);
        free(ret);
        return NULL;
    }
    struct stat statbuf;
    int err = fstat(ret->file_des, &statbuf);
    if (err < 0) {
        close(ret->file_des);
        free((void *)ret->empty_flash_mem);
        free(ret);
        return NULL;
    }
    if (S_ISBLK(statbuf.st_mode)) {
        uint64_t blk_size;
        err = ioctl(ret->file_des, BLKGETSIZE64, &blk_size);
        if (err < 0) {
            close(ret->file_des);
            free((void *)ret->empty_flash_mem);
            free(ret);
            return NULL;
        }
        else {
            statbuf.st_size = blk_size;
        }
    }

    off_t start_pos = 0;
    ret->last_offs  = statbuf.st_size;
    if (partition) {
        if (partition->end > statbuf.st_size) {
            close(ret->file_des);
            free((void *)ret->empty_flash_mem);
            free(ret);
            errno = E2BIG;
            return NULL;
        }
        else {
            start_pos      = partition->start;
            ret->last_offs = partition->end;
        }
    }
    ret->part_offs = start_pos;
    // Mount the file system
    cfg->read  = lfs_read;
    cfg->prog  = lfs_prog;
    cfg->erase = lfs_erase;
    cfg->sync  = lfs_sync;
#ifdef LFS_THREADSAFE
    cfg->lock   = lfs_lock_unlock;
    cfg->unlock = lfs_lock_unlock;
#endif
    cfg->context = ret;

    return ret;
}

int lfs_ioaccess_close(struct lfs_ioaccess_context *ctx)
{
    if (!ctx) {
        errno = EINVAL;
        return -1;
    }
    free((void *)ctx->empty_flash_mem);
    int ret = close(ctx->file_des);
    free(ctx);
    return ret;
}

int lfs_ioaccess_is_lfs_filesystem(struct lfs_ioaccess_context *ctx)
{
    static const char lfs_id[]   = "littlefs";
    static const size_t lfs_offs = 8U;
    char buf[32];
    if (!ctx) {
        errno = EINVAL;
        return -1;
    }
    off_t offs = lseek(ctx->file_des, ctx->part_offs + lfs_offs, SEEK_SET);
    if (offs < 0) {
        return -1;
    }
    else if (offs != ctx->part_offs + (off_t)lfs_offs) {
        errno = ERANGE;
        return -1;
    }
    size_t rd_req = sizeof buf;
    char *rd_buf  = buf;
    do {
        ssize_t ret = read(ctx->file_des, rd_buf, rd_req);
        if (ret > 0) {
            rd_req -= ret;
            rd_buf += ret;
        }
        else if (ret == 0) {
            break;
        }
        else {
            return ret;
        }
    } while (rd_req > 0);
    if (rd_req) {
        errno = ERANGE;
        return -1;
    }
    return memcmp(buf, lfs_id, sizeof(lfs_id) - sizeof('\0')) == 0;
}
