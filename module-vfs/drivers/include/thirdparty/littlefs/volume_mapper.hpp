// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <purefs/blkdev/defs.hpp>
#include <memory>

namespace purefs::blkdev
{
    class disk_manager;
}
struct lfs_config;

namespace purefs::fs::drivers::littlefs::internal
{

    /**
     * Append volume and configure LFS
     * @param lfs Lfs structure in the mount point
     * @param diskmm Disk manager
     * @param diskh  Disk handle in manager
     * @return Error code or success
     */
    int append_volume(lfs_config *lfsc, std::shared_ptr<blkdev::disk_manager> diskmm, blkdev::disk_fd diskh);

    /**  Deconfigure volume
     *
     * @param lfsc LFs configuration structure
     */
    void remove_volume(lfs_config *lfsc);

} // namespace purefs::fs::drivers::littlefs::internal
