// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "lfs.h"
#include "lfs_ioaccess.h"
#include "parse_partitions.h"

#include <sys/mman.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

struct lfs_ioaccess_context
{
    uint8_t *data;
    int data_fd;
    size_t mmap_size;
    struct timeval last_msync;
};

static int lfs_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    struct lfs_ioaccess_context *ctx = c->context;
    uint8_t *data                    = ctx->data;
    memcpy(buffer, data + (block * c->block_size) + off, size);
    return 0;
}

static int lfs_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    struct lfs_ioaccess_context *ctx = c->context;
    uint8_t *data                    = ctx->data;
    memcpy(data + (block * c->block_size) + off, buffer, size);
    return 0;
}

static int lfs_erase(const struct lfs_config *c, lfs_block_t block)
{
    struct lfs_ioaccess_context *ctx = c->context;
    uint8_t *data                    = ctx->data;
    memset(data + (block * c->block_size), 0, c->block_size);
    return 0;
}

static int lfs_sync(const struct lfs_config *c)
{
    // NOTES: Flush the dirty pages on disk ~1s
    struct lfs_ioaccess_context *ctx = c->context;
    struct timeval curr_msync, result_msync;
    if (gettimeofday(&curr_msync, NULL) == -1) {
        return -1;
    }
    timersub(&curr_msync, &ctx->last_msync, &result_msync);
    int err = 0;
    if (result_msync.tv_sec >= 1) {
        err             = msync(ctx->data, ctx->mmap_size, MS_ASYNC);
        ctx->last_msync = curr_msync;
    }
    return err;
}

struct lfs_ioaccess_context *lfs_ioaccess_open(struct lfs_config *cfg,
                                               const char *filename,
                                               const struct partition *partition)
{
    struct lfs_ioaccess_context *ret = calloc(1, sizeof(struct lfs_ioaccess_context));
    ret->data_fd                     = open(filename, O_RDWR);
    if (ret->data_fd < 0) {
        free(ret);
        return NULL;
    }
    struct stat statbuf;
    int err = fstat(ret->data_fd, &statbuf);
    if (err < 0) {
        close(ret->data_fd);
        free(ret);
        return NULL;
    }
    off_t start_pos = 0;
    ret->mmap_size  = statbuf.st_size;
    if (partition) {
        if (partition->end > statbuf.st_size) {
            close(ret->data_fd);
            free(ret);
            errno   = E2BIG;
            return NULL;
        }
        else {
            start_pos = partition->start;
            ret->mmap_size = partition->end - partition->start + 1;
        }
    }

    ret->data = mmap(NULL, ret->mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, ret->data_fd, start_pos);
    if (ret->data == MAP_FAILED) {
        close(ret->data_fd);
        free(ret);
        return NULL;
    }

    // Mount the file system
    cfg->read  = lfs_read;
    cfg->prog  = lfs_prog;
    cfg->erase = lfs_erase;
    cfg->sync  = lfs_sync;
    cfg->context = ret;
    return ret;
}

int lfs_ioaccess_close(struct lfs_ioaccess_context *ctx)
{
    if (!ctx) {
        errno = EINVAL;
        return -1;
    }
    if (ctx->data_fd < 0) {
        errno = EINVAL;
        return -1;
    }
    msync(ctx->data, ctx->mmap_size, MS_SYNC);
    int err = munmap(ctx->data, ctx->mmap_size);
    if (err < 0) {
        close(ctx->data_fd);
        return err;
    }
    err = close(ctx->data_fd);
    free(ctx);
    return err;
}

int lfs_ioaccess_is_lfs_filesystem(struct lfs_ioaccess_context *ctx)
{
    static const char lfs_id[] = "littlefs";
    if (!ctx) {
        errno = EINVAL;
        return -1;
    }
    if (!ctx->data) {
        errno = ENOMEM;
        return -1;
    }
    return memcmp(ctx->data + 8, lfs_id, sizeof(lfs_id) - sizeof('\0')) == 0;
}
