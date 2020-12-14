// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/fs/drivers/filesystem_littlefs.hpp>
#include <purefs/fs/drivers/mount_point_littlefs.hpp>
#include <lfs.h>

namespace purefs::fs::drivers
{

    auto filesystem_littlefs::mount_prealloc(std::shared_ptr<blkdev::internal::disk_handle> diskh,
                                             std::string_view path,
                                             unsigned int flags) -> fsmount
    {
        return std::make_shared<mount_point_littlefs>(diskh, path, flags, shared_from_this());
    }

    auto filesystem_littlefs::mount(fsmount mnt) noexcept -> int
    {
        return -1;
    }

} // namespace purefs::fs::drivers
