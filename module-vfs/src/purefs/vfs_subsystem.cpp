// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/fs/filesystem.hpp>
#include <purefs/fs/drivers/filesystem_vfat.hpp>
#include <purefs/fs/drivers/filesystem_littlefs.hpp>
#include <purefs/fs/drivers/filesystem_ext4.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/vfs_subsystem.hpp>
#include <purefs/vfs_subsystem_internal.hpp>
#include <hal/boot_control.h>
#include <purefs/fs/thread_local_cwd.hpp>
#include <log/log.hpp>
#include <purefs/filesystem_paths.hpp>
#include <fcntl.h>

namespace purefs::subsystem
{
    namespace
    {
        constexpr auto default_blkdev_name      = "emmc0";
        constexpr auto default_nvrom_name       = "nvrom0";
        constexpr auto fat_part_code            = 0x0B;
        constexpr auto lfs_part_code            = 0x9E;
        constexpr auto linux_part_code          = 0x83;
        constexpr auto layout_part_count        = 3;
        constexpr auto boot_part_index          = 0;
        constexpr auto user_part_index          = 2;
        constexpr auto boot_size_limit          = 16384L;
        constexpr auto block_size_max_shift     = 21;
        constexpr auto block_size_min_shift     = 8;
        constexpr uint32_t nvrom_lfs_block_size = 128U;
        namespace json
        {
            constexpr auto os_type = "ostype";
            constexpr auto main    = "main";
        } // namespace json
        std::weak_ptr<blkdev::disk_manager> g_disk_mgr;
        std::weak_ptr<fs::filesystem> g_fs_core;
    } // namespace

    auto initialize(std::unique_ptr<DeviceFactory> deviceFactory)
        -> std::tuple<std::shared_ptr<blkdev::disk_manager>, std::shared_ptr<fs::filesystem>>
    {
        auto disk_mgr                            = std::make_shared<blkdev::disk_manager>();
        const std::shared_ptr<blkdev::disk> bdev = deviceFactory->makeDefaultBlockDevice();
        auto err                                 = disk_mgr->register_device(bdev, default_blkdev_name);
        if (err) {
            LOG_FATAL("Unable to register block device with error %i", err);
            return {};
        }
        const std::shared_ptr<blkdev::disk> nvrom_bdev = deviceFactory->makeDefaultNvmDevice();
        if (nvrom_bdev) {
            err = disk_mgr->register_device(nvrom_bdev, default_nvrom_name, blkdev::flags::no_parts_scan);
            if (err) {
                LOG_WARN("Unable to register NVROM device err %i. Possible: NVROM unavailable", err);
            }
        }
        else {
            LOG_WARN("No NVROM driver available for this platform");
        }
        auto fs_core = std::make_shared<fs::filesystem>(disk_mgr);
        err          = fs_core->register_filesystem("vfat", std::make_shared<fs::drivers::filesystem_vfat>());
        if (err) {
            LOG_FATAL("Unable to register vfat filesystem with error %i", err);
            return {};
        }
        err = fs_core->register_filesystem("littlefs", std::make_shared<fs::drivers::filesystem_littlefs>());
        if (err) {
            LOG_FATAL("Unable to register lfs filesystem with error %i", err);
            return {};
        }
        err = fs_core->register_filesystem("ext4", std::make_shared<fs::drivers::filesystem_ext4>());
        if (err) {
            LOG_FATAL("Unable to register ext4 filesystem with error %i", err);
            return {};
        }

        g_disk_mgr = disk_mgr;
        g_fs_core  = fs_core;
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

    auto mount_defaults() -> int
    {
        auto disk = g_disk_mgr.lock();
        if (!disk) {
            LOG_FATAL("Unable to lock disk");
            return -EIO;
        }
        const auto parts = disk->partitions(default_blkdev_name);
        if (parts.size() != layout_part_count) {
            LOG_FATAL("Unknown partitions layout part size is %u", (unsigned)(parts.size()));
            return -EIO;
        }

        auto vfs = g_fs_core.lock();
        if (!vfs) {
            LOG_FATAL("Unable to lock vfs core");
            return -EIO;
        }

        const auto &user_part = parts[user_part_index];

        auto ret = vfs->mount(user_part.name, purefs::dir::getUserDiskPath().string(), "auto");
        if (ret != 0) {
            LOG_FATAL("Failed to mount user partition on '%s'!", purefs::dir::getUserDiskPath().c_str());
            return -EIO;
        }

        ret = boot_control_init(purefs::dir::getBootJSONPath().string().c_str());
        if (ret != 0) {
            LOG_FATAL("Unable to init boot.json handling!");
            return -ENOENT;
        }

        const auto &boot_part = parts[(get_current_slot() == Slot_A) ? 0 : 1];
        if ((boot_part.type != fat_part_code) && (boot_part.type != linux_part_code)) {
            LOG_FATAL("Invalid boot partition type expected code: %02X or %02X current code: %02X",
                      fat_part_code,
                      linux_part_code,
                      boot_part.type);
            return -EIO;
        }
        if (user_part.type != linux_part_code) {
            LOG_FATAL(
                "Invalid user partition type expected code: %02X current code: %02X", linux_part_code, user_part.type);
            return -EIO;
        }

        auto err = vfs->mount(boot_part.name, purefs::dir::getSystemDiskPath().string(), "auto");
        if (err != 0) {
            return err;
        }

        fs::internal::set_default_thread_cwd(purefs::dir::getSystemDiskPath().string());

        // Mount NVRAM memory
        err = vfs->mount(default_nvrom_name,
                         purefs::dir::getMfgConfPath().c_str(),
                         "littlefs",
                         fs::mount_flags::read_only,
                         &nvrom_lfs_block_size);
        if (err != 0) {
            LOG_WARN("Unable to mount NVROM partition err %i. Possible: NVROM unavailable", err);
            err = 0;
        }
        return err;
    }

    auto unmount_all() -> int
    {
        auto vfs = g_fs_core.lock();
        if (!vfs) {
            LOG_ERROR("Unable to lock vfs");
            return -EIO;
        }
        std::list<std::string> mount_points;
        int err = vfs->read_mountpoints(mount_points);
        if (err) {
            return err;
        }
        for (const auto &mpoint : mount_points) {
            err = vfs->umount(mpoint);
            if (err)
                break;
        }
        boot_control_deinit();
        return err;
    }

} // namespace purefs::subsystem
