// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <memory>
#include <purefs/fs/filesystem.hpp>
#include <purefs/blkdev/disk_manager.hpp>

namespace purefs::subsystem
{
    auto initialize() -> std::tuple<std::shared_ptr<blkdev::disk_manager>, std::shared_ptr<fs::filesystem>>;
    auto disk_mgr() -> std::shared_ptr<blkdev::disk_manager>;
    auto vfs_core() -> std::shared_ptr<fs::filesystem>;
    auto mount_defaults() -> int;
} // namespace purefs::subsystem
