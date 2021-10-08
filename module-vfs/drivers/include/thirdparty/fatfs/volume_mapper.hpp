// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
     * @param[in] disk Disk manager handle for attach partition to ff_fat
     * @return FF volume identifier or error if negative
     */
    int append_volume(blkdev::disk_fd diskh);

    /** Clear mapping table for FF_FAT logical drives
     * @param[in] diskmm Initialized disk manager object
     */
    void reset_volumes(std::shared_ptr<blkdev::disk_manager> diskmm);

    /** Remove volume and drive from ff_fat layer
     * @param [in] diskh Disk manager handle object
     * @return 0 on success otherwise error
     */
    int remove_volume(blkdev::disk_fd diskh);

} // namespace purefs::fs::drivers::ffat::internal
