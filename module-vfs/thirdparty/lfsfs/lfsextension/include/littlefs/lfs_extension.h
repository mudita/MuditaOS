// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/* NOTE: Currently littlefs doesn't support auto detecting block_size and block_count
 * basing on the data stored in the the super block of formatted filesystem.
 * It expect data passed during filesystem initialization.
 * This extension implements really naive algorithm for finding first
 *superblock entry and read block_size and blocks count from the super block
 */

#pragma once

#include <littlefs/lfs.h>

#ifdef __cplusplus
extern "C"
{
#endif
    /**
     * Try to setup config structure based on the superblock information
     * @param cfg LFS config structure
     * @return 0 or success otherwise error
     */
    int lfs_extension_read_config_from_superblock(struct lfs_config *cfg, size_t sector_size)
        __attribute__((nonnull(1)));

#ifdef __cplusplus
}
#endif
