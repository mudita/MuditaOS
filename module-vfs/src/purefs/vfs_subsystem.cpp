// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/fs/filesystem.hpp>
#include <purefs/fs/drivers/filesystem_vfat.hpp>
#include <purefs/fs/drivers/filesystem_littlefs.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/vfs_subsystem.hpp>
#include <purefs/vfs_subsystem_internal.hpp>
#include <purefs/fs/thread_local_cwd.hpp>
#include <log/log.hpp>
#include <purefs/filesystem_paths.hpp>
#include <module-utils/json/json11.hpp>
#include <sys/stat.h>
#include <fcntl.h>

namespace purefs::subsystem
{
    namespace
    {
        constexpr auto default_blkdev_name = "emmc0";
        constexpr auto fat_part_code         = 0x0b;
        constexpr auto lfs_part_code         = 0x9e;
        constexpr auto old_layout_part_count = 2;
        constexpr auto new_layout_part_count = 3;
        constexpr auto boot_size_limit       = 16384L;
        constexpr auto block_size_max_shift  = 21;
        namespace json
        {
            constexpr auto os_type = "ostype";
            constexpr auto main    = "main";
        } // namespace json
        std::weak_ptr<blkdev::disk_manager> g_disk_mgr;
        std::weak_ptr<fs::filesystem> g_fs_core;
    } // namespace

    namespace
    {
        int read_file_to_cpp_string(std::shared_ptr<fs::filesystem> vfs, std::string_view file, std::string &str)
        {
            int fd = vfs->open(file, O_RDONLY, 0664);
            if (fd < 0) {
                return fd;
            }
            size_t rd_pos  = 0;
            size_t to_read = str.size();
            do {
                int err = vfs->read(fd, &str[rd_pos], to_read);
                if (err < 0) {
                    vfs->close(fd);
                    return err;
                }
                else {
                    to_read -= err;
                    rd_pos += err;
                }
            } while (to_read > 0);
            return vfs->close(fd);
        }

        std::string parse_boot_json_directory(std::string_view file)
        {
            using namespace std::literals;
            auto vfsn = g_fs_core.lock();
            if (!vfsn) {
                LOG_ERROR("Unable to lock vfs. Fallback to current dir");
                return "current"s;
            }
            struct stat stbuf;
            int err = vfsn->stat(file, stbuf);
            if (err) {
                LOG_ERROR("Unable to lock vfs fallback to current dir");
                return "current"s;
            }
            if (stbuf.st_size > boot_size_limit) {
                LOG_ERROR("Boot file to long fallback to current dir");
                return "current"s;
            }
            std::string json_str(stbuf.st_size, ' ');
            std::string error;
            err = read_file_to_cpp_string(vfsn, file, json_str);
            if (err) {
                LOG_ERROR("Unable to read boot file fallback to current dir err %i", err);
                return "current"s;
            }
            auto json = json11::Json::parse(json_str, error);
            if (!error.empty()) {
                LOG_ERROR("Unable to parse json boot file fallback to current dir error %s", error.c_str());
                return "current"s;
            }
            return json[json::main][json::os_type].string_value();
        }
    } // namespace

    auto initialize() -> std::tuple<std::shared_ptr<blkdev::disk_manager>, std::shared_ptr<fs::filesystem>>
    {
        auto disk_mgr   = std::make_shared<blkdev::disk_manager>();
        const auto bdev = internal::create_default_block_device();
        auto err        = disk_mgr->register_device(bdev, default_blkdev_name);
        if (err) {
            LOG_FATAL("Unable to register block device with error %i", err);
            return {};
        }
        auto fs_core = std::make_shared<fs::filesystem>(disk_mgr);
        err          = fs_core->register_filesystem("vfat", std::make_shared<fs::drivers::filesystem_vfat>());
        if (err) {
            LOG_FATAL("Unable to register vfat filesystem with error %i", err);
            return {};
        }
        err = fs_core->register_filesystem("littlefs", std::make_shared<fs::drivers::filesystem_littlefs>());
        if (err) {
            LOG_FATAL("Unable to register vfat filesystem with error %i", err);
            return {};
        }

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

    auto mount_defaults() -> int
    {
        auto disk = g_disk_mgr.lock();
        if (!disk) {
            LOG_FATAL("Unable to lock disk");
            return -EIO;
        }
        auto parts = disk->partitions(default_blkdev_name);
        if (parts.size() != old_layout_part_count && parts.size() != new_layout_part_count) {
            LOG_FATAL("Unknown partitions layout part size is %u", unsigned(parts.size()));
            return -EIO;
        }
        auto boot_it = std::end(parts);
        auto lfs_it  = std::end(parts);
        for (auto it = std::begin(parts); it != std::end(parts); ++it) {
            if (it->bootable && boot_it == std::end(parts)) {
                boot_it = it;
            }
            else if (it->type == lfs_part_code && lfs_it == std::end(parts)) {
                lfs_it = it;
            }
        }
        if (lfs_it == std::end(parts) && parts.size() == old_layout_part_count) {
            LOG_ERROR("!!!! Caution !!!! eMMC is formated with vFAT old layout scheme. Filesystem may be currupted on "
                      "power loss.");
            LOG_WARN("Please upgrade to new partition scheme based on the LittleFS filesystem.");
        }
        if (boot_it == std::end(parts)) {
            LOG_FATAL("Unable to find boot partition");
            return -ENOENT;
        }
        auto vfs = g_fs_core.lock();
        if (!vfs) {
            LOG_FATAL("Unable to lock vfs core");
            return -EIO;
        }
        auto err = vfs->mount(boot_it->name, purefs::dir::getRootDiskPath().string(), "vfat");
        if (err) {
            return err;
        }
        if (lfs_it != std::end(parts)) {
            if (lfs_it->boot_unit > block_size_max_shift) {
                LOG_FATAL("Boot sector size is out of range");
                return -ERANGE;
            }
            else {
                const uint32_t lfs_block_size = 1U << lfs_it->boot_unit;
                const auto lfs_block_size_ptr = (lfs_it->boot_unit) ? (&lfs_block_size) : nullptr;
                err                           = vfs->mount(
                    lfs_it->name, purefs::dir::getUserDiskPath().string(), "littlefs", 0, lfs_block_size_ptr);
            }
        }
        const std::string json_file = (dir::getRootDiskPath() / file::boot_json).string();
        const auto boot_dir_name    = parse_boot_json_directory(json_file);
        const auto user_dir         = (dir::getRootDiskPath() / boot_dir_name).string();
        fs::internal::set_default_thread_cwd(user_dir);
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
        return err;
    }

} // namespace purefs::subsystem
