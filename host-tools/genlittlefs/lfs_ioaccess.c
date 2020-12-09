// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "lfs.h"
#include "lfs_ioaccess.h"
#include "parse_partitions.h"

#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

static uint8_t *data = NULL;
static int data_fd   = -1;
static mmap_size     = 0;

static int lfs_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    memcpy(buffer, data + (block * c->block_size) + off, size);
    return 0;
}

static int lfs_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    memcpy(data + (block * c->block_size) + off, buffer, size);
    return 0;
}

static int lfs_erase(const struct lfs_config *c, lfs_block_t block)
{
    memset(data + (block * c->block_size), 0, c->block_size);
    return 0;
}

static int lfs_sync(const struct lfs_config *c)
{
    return 0;
}

int lfs_ioaccess_open(struct lfs_config *cfg, const char *filename, const struct partition *partition)
{
    if (data_fd > 0) {
        errno = EINVAL;
        return -1;
    }
    data_fd = open(filename, O_RDWR);
    if (data_fd < 0) {
        return data_fd;
    }
    struct stat statbuf;
    int err = fstat(data_fd, &statbuf);
    if (err < 0) {
        close(data_fd);
        data_fd = -1;
        return err;
    }
    off_t start_pos = 0;
    mmap_size       = statbuf.st_size;
    if (partition) {
        if (partition->end > statbuf.st_size) {
            close(data_fd);
            data_fd = -1;
            errno   = E2BIG;
            return -1;
        }
        else {
            start_pos = partition->start;
            mmap_size = partition->end - partition->start;
        }
    }
    err = lseek(data_fd, start_pos, SEEK_SET);
    if (err < 0) {
        close(data_fd);
        data_fd = -1;
        return err;
    }
    data = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, data_fd, 0);
    if (data == MAP_FAILED) {
        close(data_fd);
        data_fd = -1;
        return -1;
    }
    // Mount the file system
    cfg->read  = lfs_read;
    cfg->prog  = lfs_prog;
    cfg->erase = lfs_erase;
    cfg->sync  = lfs_sync;
    return 0;
}

int lfs_ioaccess_close()
{
    if (data_fd < 0) {
        errno = EINVAL;
        return -1;
    }
    int err = munmap(data, mmap_size);
    if (err < 0) {
        close(data_fd);
        data_fd = -1;
        return err;
    }
    close(data_fd);
    data      = NULL;
    mmap_size = 0;
    return 0;
}
