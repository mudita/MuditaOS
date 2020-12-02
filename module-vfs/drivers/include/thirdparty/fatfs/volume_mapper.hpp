// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>
#include <memory>
#include <purefs/blkdev/defs.hpp>

namespace purefs::blkdev
{
    class disk_manager;
}
namespace purefs::fs::drivers::ffat::internal
{

    /** Map volume to partion for FF_FAT logical drive
     */
    int append_volume(blkdev::disk_fd disk);
    /** Clear mapping table for FF_FAT logical drives
     */
    void reset_volumes(std::shared_ptr<blkdev::disk_manager> diskmm);
} // namespace purefs::fs::drivers::ffat::internal
