// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <memory>
#include <purefs/fs/filesystem.hpp>
#include <purefs/blkdev/disk_manager.hpp>

namespace purefs::subsystem
{
    using vfs_handle_t = std::tuple<std::shared_ptr<blkdev::disk_manager>, std::shared_ptr<fs::filesystem>>;
    auto initialize() -> vfs_handle_t;
    auto disk_mgr() -> std::shared_ptr<blkdev::disk_manager>;
    auto vfs_core() -> std::shared_ptr<fs::filesystem>;
    auto mount_defaults() -> int;
    auto unmount_all() -> int;
} // namespace purefs::subsystem
