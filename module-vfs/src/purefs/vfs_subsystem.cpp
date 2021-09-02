// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <purefs/fs/filesystem.hpp>
#include <purefs/fs/drivers/filesystem_vfat.hpp>
#include <purefs/fs/drivers/filesystem_littlefs.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/vfs_subsystem.hpp>
#include <purefs/vfs_subsystem_internal.hpp>
#include <purefs/fs/thread_local_cwd.hpp>
#include <log.hpp>
#include <purefs/filesystem_paths.hpp>
#include <json11.hpp>
#include <sys/stat.h>
#include <fcntl.h>

namespace purefs::subsystem
{
    namespace
    {
        constexpr auto default_blkdev_name      = "emmc0";
        constexpr auto default_nvrom_name       = "nvrom0";
        constexpr auto fat_part_code            = 0x0b;
        constexpr auto lfs_part_code            = 0x9e;
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
                LOG_ERROR("Unable to lock vfs. Fallback to current dir");
                return "current"s;
            }
            if (stbuf.st_size > boot_size_limit) {
                LOG_ERROR("Boot file to long. Fallback to current dir");
                return "current"s;
            }
            std::string json_str(stbuf.st_size, ' ');
            std::string error;
            err = read_file_to_cpp_string(vfsn, file, json_str);
            if (err) {
                LOG_ERROR("Unable to read boot file (err: %i). Fallback to current dir", err);
                return "current"s;
            }
            auto json = json11::Json::parse(json_str, error);
            if (!error.empty()) {
                LOG_ERROR("Unable to parse json boot file (err: %s). Fallback to current dir", error.c_str());
                return "current"s;
            }
            return json[json::main][json::os_type].string_value();
        }

        int read_mbr_lfs_erase_size(std::shared_ptr<blkdev::disk_manager> disk_mngr,
                                    std::string_view dev_name,
                                    int part_no)
        {
            static constexpr auto MBR_ERASE_BLK_OFFSET = 0x00E0;
            if (part_no <= 0) {
                return -EINVAL;
            }
            const auto sect_size = disk_mngr->get_info(dev_name, blkdev::info_type::sector_size);
            if (sect_size <= MBR_ERASE_BLK_OFFSET + part_no) {
                return (sect_size > 0) ? (-ERANGE) : (sect_size);
            }
            auto mbr_buf = std::make_unique<char[]>(sect_size);
            int err      = disk_mngr->read(dev_name, mbr_buf.get(), 0, 1);
            if (err < 0) {
                return err;
            }
            return mbr_buf[MBR_ERASE_BLK_OFFSET + part_no];
        }

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
            LOG_FATAL("Unknown partitions layout part size is %u", unsigned(parts.size()));
            return -EIO;
        }
        const auto &boot_part = parts[boot_part_index];
        const auto &user_part = parts[user_part_index];
        if (!boot_part.bootable) {
            LOG_FATAL("First partition is not bootable");
            return -EIO;
        }
        if (boot_part.type != fat_part_code) {
            LOG_FATAL("Invalid boot partition type expected code: %i current code: %i", fat_part_code, boot_part.type);
            return -EIO;
        }
        if (user_part.type != lfs_part_code) {
            LOG_FATAL("Invalid user partition type expected code: %i current code: %i", lfs_part_code, user_part.type);
            return -EIO;
        }
        auto vfs = g_fs_core.lock();
        if (!vfs) {
            LOG_FATAL("Unable to lock vfs core");
            return -EIO;
        }
        auto err =
            vfs->mount(boot_part.name, purefs::dir::getRootDiskPath().string(), "vfat", fs::mount_flags::read_only);
        if (err) {
            return err;
        }
        const int lfs_block_log2     = read_mbr_lfs_erase_size(disk, default_blkdev_name, user_part.physical_number);
        uint32_t lfs_block_size      = 0;
        uint32_t *lfs_block_size_ptr = nullptr;
        if (lfs_block_log2 >= block_size_min_shift && lfs_block_log2 <= block_size_max_shift) {
            lfs_block_size     = 1U << lfs_block_log2;
            lfs_block_size_ptr = &lfs_block_size;
        }
        err = vfs->mount(user_part.name, purefs::dir::getUserDiskPath().string(), "littlefs", 0, lfs_block_size_ptr);
        const std::string json_file = (dir::getRootDiskPath() / file::boot_json).string();
        const auto boot_dir_name    = parse_boot_json_directory(json_file);
        const auto user_dir         = (dir::getRootDiskPath() / boot_dir_name).string();
        fs::internal::set_default_thread_cwd(user_dir);

        // Mount NVRAM memory
        err = vfs->mount(default_nvrom_name,
                         purefs::dir::getMfgConfPath().c_str(),
                         "littlefs",
                         fs::mount_flags::read_only,
                         &nvrom_lfs_block_size);
        if (err) {
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
        return err;
    }

} // namespace purefs::subsystem
