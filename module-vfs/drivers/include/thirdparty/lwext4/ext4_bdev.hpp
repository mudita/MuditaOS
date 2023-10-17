// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <purefs/blkdev/defs.hpp>

namespace purefs::blkdev
{
    class disk_manager;
}

struct ext4_blockdev;

namespace purefs::fs::drivers::ext4::internal
{
    /**
     * Append volume and return ext4 block device
     * @param diskmm Disk manager
     * @param diskh Disk handle in the manager
     * @return block device and error code
     */
    std::pair<ext4_blockdev *, int> append_volume(std::shared_ptr<blkdev::disk_manager> diskmm, blkdev::disk_fd diskh);

    /** Remove ext4 block device from the table
     * @param ext4_block Ext4 block device for free
     * @return error code
     */
    int remove_volume(ext4_blockdev *ext4_block);

} // namespace purefs::fs::drivers::ext4::internal
