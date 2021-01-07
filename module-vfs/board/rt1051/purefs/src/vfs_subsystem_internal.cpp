// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/vfs_subsystem_internal.hpp>
#include <purefs/blkdev/disk_manager.hpp>

namespace purefs::subsystem::internal
{
    auto create_default_block_device() -> std::shared_ptr<blkdev::disk>
    {
        // TODO: implement for rt1051
        return nullptr;
    }
} // namespace purefs::subsystem::internal
