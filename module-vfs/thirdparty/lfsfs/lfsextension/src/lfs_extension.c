// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "lfs_extension.h"
#include "lfs_util.h"

static inline void lfs_superblock_fromle32(lfs_superblock_t *superblock)
{
    superblock->version     = lfs_fromle32(superblock->version);
    superblock->block_size  = lfs_fromle32(superblock->block_size);
    superblock->block_count = lfs_fromle32(superblock->block_count);
    superblock->name_max    = lfs_fromle32(superblock->name_max);
    superblock->file_max    = lfs_fromle32(superblock->file_max);
    superblock->attr_max    = lfs_fromle32(superblock->attr_max);
}

static void *memstr(const void *mem, size_t memsize, const char *str)
{
    const size_t findstr_len = strlen(str);
    const uint8_t *memu8p    = mem;
    if (findstr_len > memsize) {
        return NULL;
    }
    uint8_t *match = memchr(memu8p, str[0], memsize);
    if (match != NULL) {
        size_t remaining = memsize - ((uint8_t *)match - memu8p);
        if (findstr_len <= remaining) {
            if (memcmp(match, str, findstr_len) == 0) {
                return match;
            }
        }
    }
    return NULL;
}

int lfs_extension_read_config_from_superblock(struct lfs_config *cfg, size_t sector_size)
{
    uint8_t sect_buf[sector_size] __attribute__((aligned));
    if (!cfg->read) {
        return LFS_ERR_IO;
    }
    lfs_superblock_t *lfs_superblock;
    if (sector_size <= sizeof(*lfs_superblock)) {
        return LFS_ERR_IO;
    }
    lfs_size_t block_size_saved = cfg->block_size;
    int err                     = cfg->read(cfg, 0, 0, sect_buf, sector_size);
    cfg->block_size             = block_size_saved;
    if (err) {
        return err;
    }
    const uint8_t *ptr = memstr(sect_buf, sizeof sect_buf, "littlefs");
    if (!ptr) {
        return LFS_ERR_NOENT;
    }
    lfs_superblock = (lfs_superblock_t *)(ptr + 12);
    lfs_superblock_fromle32(lfs_superblock);
    // check version
    uint16_t major_version = (0xffff & (lfs_superblock->version >> 16));
    uint16_t minor_version = (0xffff & (lfs_superblock->version >> 0));
    if ((major_version != LFS_DISK_VERSION_MAJOR || minor_version > LFS_DISK_VERSION_MINOR)) {
        LFS_ERROR("Invalid version v%" PRIu16 ".%" PRIu16, major_version, minor_version);
        return LFS_ERR_INVAL;
    }
    cfg->block_size  = lfs_superblock->block_size;
    cfg->block_count = lfs_superblock->block_count;
    return LFS_ERR_OK;
}
