// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/fs/filesystem.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/vfs_subsystem.hpp>
#include <purefs/vfs_subsystem_internal.hpp>
#include <log/log.hpp>

namespace purefs::subsystem
{
    namespace
    {
        constexpr auto default_blkdev_name = "emmc0";

        std::weak_ptr<blkdev::disk_manager> g_disk_mgr;
        std::weak_ptr<fs::filesystem> g_fs_core;
    } // namespace

    auto initialize() -> std::tuple<std::shared_ptr<blkdev::disk_manager>, std::shared_ptr<fs::filesystem>>
    {
        auto disk_mgr   = std::make_shared<blkdev::disk_manager>();
        const auto bdev = internal::create_default_block_device();
        auto err        = disk_mgr->register_device(bdev, default_blkdev_name);
        if (err) {
            LOG_FATAL("Unable to register block device with error %i", err);
            std::abort();
        }
        auto fs_core = std::make_shared<fs::filesystem>(g_disk_mgr.lock());
        g_disk_mgr   = disk_mgr;
        g_fs_core    = fs_core;
        return {disk_mgr, fs_core};
    }

    auto disk_mgr() -> std::shared_ptr<blkdev::disk_manager>
    {
        return g_disk_mgr.lock();
    }

    auto vfs_core() -> std::shared_ptr<fs::filesystem>
    {
        return g_fs_core.lock();
    }
} // namespace purefs::subsystem
