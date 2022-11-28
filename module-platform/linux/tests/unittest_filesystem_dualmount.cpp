// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <platform/linux/DiskImage.hpp>

#include <purefs/fs/filesystem.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/fs/drivers/filesystem_ext4.hpp>
#include <purefs/fs/drivers/filesystem_littlefs.hpp>

#include "test-setup.hpp"

#include <cstring>
#include <sys/statvfs.h>
#include <fcntl.h>

namespace
{
    constexpr auto lfs_disk_image = "lfstest.img";

    auto prepare_filesystem(std::string_view vfat_dev_name, std::string_view lfs_dev_name)
        -> std::pair<std::unique_ptr<purefs::fs::filesystem>, std::shared_ptr<purefs::blkdev::disk_manager>>
    {
        using namespace purefs;

        auto dm        = std::make_shared<blkdev::disk_manager>();
        auto vfat_disk = std::make_shared<blkdev::disk_image>(::testing::vfs::disk_image);
        auto lfs_disk  = std::make_shared<blkdev::disk_image>(lfs_disk_image);

        if (dm->register_device(vfat_disk, vfat_dev_name) != 0) {
            return {};
        }
        if (dm->register_device(lfs_disk, lfs_dev_name) != 0) {
            return {};
        }

        auto fs_core        = std::make_unique<fs::filesystem>(dm);
        const auto vfs_ext4 = std::make_shared<fs::drivers::filesystem_ext4>();
        const auto vfs_lfs  = std::make_shared<fs::drivers::filesystem_littlefs>();

        if (fs_core->register_filesystem("ext4", vfs_ext4) != 0) {
            return {};
        }
        if (fs_core->register_filesystem("littlefs", vfs_lfs) != 0) {
            return {};
        }

        return std::make_pair(std::move(fs_core), std::move(dm));
    }
} // namespace

TEST_CASE("dualmount: Basic mount")
{
    using namespace purefs;

    auto dm        = std::make_shared<blkdev::disk_manager>();
    auto ext4_disk = std::make_shared<blkdev::disk_image>(::testing::vfs::disk_image);
    auto lfs_disk  = std::make_shared<blkdev::disk_image>(lfs_disk_image);
    REQUIRE(ext4_disk);
    REQUIRE(lfs_disk);
    REQUIRE(dm->register_device(ext4_disk, "ext40") == 0);
    REQUIRE(dm->register_device(lfs_disk, "lfs0") == 0);

    fs::filesystem fs_core(dm);
    const auto vfs_ext4 = std::make_shared<fs::drivers::filesystem_ext4>();
    const auto vfs_lfs  = std::make_shared<fs::drivers::filesystem_littlefs>();
    REQUIRE(vfs_ext4->mount_count() == 0);
    REQUIRE(vfs_lfs->mount_count() == 0);
    REQUIRE(fs_core.register_filesystem("ext4", vfs_ext4) == 0);
    REQUIRE(fs_core.register_filesystem("littlefs", vfs_lfs) == 0);

    REQUIRE(fs_core.mount("ext40part0", "/ext4", "ext4") == 0);
    REQUIRE(fs_core.mount("lfs0part0", "/lfs", "littlefs") == 0);
    REQUIRE(vfs_ext4->mount_count() == 1);
    REQUIRE(vfs_lfs->mount_count() == 1);
    REQUIRE(fs_core.umount("/ext4") == 0);
    REQUIRE(fs_core.umount("/lfs") == 0);
}

TEST_CASE("dualmount: API tests")
{
    auto [fs_core, dm] = prepare_filesystem("ext40", "lfs0");
    REQUIRE(fs_core->mount("ext40part0", "/ext4", "ext4") == 0);
    REQUIRE(fs_core->mount("lfs0part0", "/lfs", "littlefs") == 0);

    SECTION("Files stat")
    {
        const auto ext4_fd = fs_core->open("/ext4/assets/fonts/fontmap.json", O_RDONLY, 0);
        REQUIRE(ext4_fd >= 3);
        const auto lfs_fd = fs_core->open("/lfs/test_read_1.txt", O_RDONLY, 0);
        REQUIRE(lfs_fd >= 3);

        struct stat ext4_file_stat
        {};
        struct stat lfs_file_stat
        {};
        REQUIRE(fs_core->fstat(ext4_fd, ext4_file_stat) == 0);
        REQUIRE(fs_core->fstat(lfs_fd, lfs_file_stat) == 0);
        REQUIRE(ext4_file_stat.st_dev != lfs_file_stat.st_dev);

        REQUIRE(fs_core->close(ext4_fd) == 0);
        REQUIRE(fs_core->close(lfs_fd) == 0);
    }
    SECTION("Create, read & write files")
    {
        const auto lfs_fd = fs_core->open("/lfs/test_read_1.txt", O_RDONLY, 0);
        REQUIRE(lfs_fd >= 3);

        static constexpr auto ext4_filename = "/ext4/test_write_tmp_1.txt";
        auto ext4_fd                        = fs_core->open(ext4_filename, O_CREAT | O_RDWR, 0);
        REQUIRE(ext4_fd >= 3);

        const std::string expected = "01234567";

        {
            char buf[64];
            REQUIRE(fs_core->read(lfs_fd, buf, expected.size()) == static_cast<ssize_t>(expected.size()));
            REQUIRE(memcmp(buf, expected.c_str(), expected.size()) == 0);
            REQUIRE(fs_core->write(ext4_fd, buf, expected.size()) == static_cast<ssize_t>(expected.size()));
            REQUIRE(fs_core->close(ext4_fd) == 0);
        }

        {
            char buf[64];
            ext4_fd = fs_core->open(ext4_filename, O_RDWR, 0);
            REQUIRE(ext4_fd >= 3);
            REQUIRE(fs_core->read(ext4_fd, buf, expected.size()) == static_cast<ssize_t>(expected.size()));
            REQUIRE(memcmp(buf, expected.c_str(), expected.size()) == 0);
            REQUIRE(fs_core->close(ext4_fd) == 0);
        }

        REQUIRE(fs_core->close(lfs_fd) == 0);
        fs_core->unlink(ext4_filename);
    }

    REQUIRE(fs_core->umount("/ext4") == 0);
    REQUIRE(fs_core->umount("/lfs") == 0);
}
