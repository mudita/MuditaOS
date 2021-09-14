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
namespace purefs::fs::drivers::reedgefs::internal
{

    /** Map volume to partion of logical drive
     * @param[in] disk Disk manager handle to attach partition
     * @return Volume identifier or error if negative
     */
    std::pair<std::string, int> append_volume(blkdev::disk_fd diskh, const std::string &path);

    /** Clear mapping table of logical drives
     * @param[in] diskmm Initialized disk manager object
     */
    void reset_volumes(std::shared_ptr<blkdev::disk_manager> diskmm);

    /** Remove volume from map
     * @param [in] diskh Disk manager handle object
     * @return 0 on success otherwise error
     */
    int remove_volume(blkdev::disk_fd diskh);

} // namespace purefs::fs::drivers::reedgefs::internal
