// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <platform/linux/DiskImage.hpp>

#include <purefs/fs/filesystem.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/fs/drivers/filesystem_ext4.hpp>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>

namespace
{
    constexpr auto disk_image = "ext4test.img";

#if 0
    auto prepare_filesystem(std::string_view dev_name)
        -> std::pair<std::unique_ptr<purefs::fs::filesystem>, std::shared_ptr<purefs::blkdev::disk_manager>>
    {
        using namespace purefs;

        auto dm   = std::make_shared<blkdev::disk_manager>();
        auto disk = std::make_shared<blkdev::disk_image>(disk_image);

        if (dm->register_device(disk, dev_name) != 0) {
            return {};
        }

        auto fs_core       = std::make_unique<fs::filesystem>(dm);
        const auto vfs_lfs = std::make_shared<fs::drivers::filesystem_ext4>();

        if (fs_core->register_filesystem("ext4", vfs_lfs) != 0) {
            return {};
        }

        return std::make_pair(std::move(fs_core), std::move(dm));
    }
#endif
} // namespace

TEST_CASE("ext4: Basic mount and functionality")
{
    using namespace purefs;

    auto dm   = std::make_shared<blkdev::disk_manager>();
    auto disk = std::make_shared<blkdev::disk_image>(disk_image);
    REQUIRE(disk);
    REQUIRE(dm->register_device(disk, "emmc0") == 0);

    fs::filesystem fscore(dm);
    const auto vfs_ext = std::make_shared<fs::drivers::filesystem_ext4>();
    REQUIRE(vfs_ext->mount_count() == 0);
    REQUIRE(fscore.register_filesystem("ext4", vfs_ext) == 0);

    REQUIRE(fscore.mount("emmc0part0", "/sys", "ext4") == 0);
    REQUIRE(vfs_ext->mount_count() == 1);

    SECTION("Mount non-existent partition")
    {
        REQUIRE(fscore.mount("dummy0part0", "/dummy", "ext4") == -ENXIO);
    }
    SECTION("Umount not mounted point")
    {
        REQUIRE(fscore.umount("/ala") == -ENOENT);
    }
    SECTION("Mount when already mounted")
    {
        REQUIRE(fscore.mount("emmc0part0", "/sys", "vfat") == -EBUSY);
    }
    SECTION("Mount invalid filesystem type")
    {
        REQUIRE(fscore.mount("emmc0part0", "/path", "vfat") == -EBUSY);
    }
    SECTION("statvfs")
    {
        struct statvfs ssv;
        REQUIRE(fscore.stat_vfs("/sys/", ssv) == 0);
    }

    REQUIRE(fscore.umount("/sys") == 0);
    REQUIRE(vfs_ext->mount_count() == 0);

    REQUIRE(fscore.mount("emmc0part0", "/path", "ext4") == 0);
    REQUIRE(vfs_ext->mount_count() == 1);
    REQUIRE(fscore.umount("/path") == 0);
}
