// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/vfs_subsystem_internal.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/blkdev/disk_image.hpp>

namespace purefs::subsystem::internal
{
    namespace
    {
        constexpr auto disk_image_name = "PurePhone.img";
    }
    auto create_default_block_device() -> std::shared_ptr<blkdev::disk>
    {
        return std::make_shared<blkdev::disk_image>(disk_image_name);
    }
} // namespace purefs::subsystem::internal
