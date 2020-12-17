// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "lfs.h"

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
