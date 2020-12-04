// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <purefs/fs/filesystem.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/blkdev/disk_image.hpp>
#include <purefs/fs/drivers/filesystem_vfat.hpp>
#include <sys/statvfs.h>

namespace
{
    constexpr auto disk_image = "PurePhone.img";
}

TEST_CASE("Corefs: Registering and unregistering block device")
{
    using namespace purefs;
    auto dm   = std::make_shared<blkdev::disk_manager>();
    auto disk = std::make_shared<blkdev::disk_image>(disk_image);
    REQUIRE(disk);
    REQUIRE(dm->register_device(disk, "emmc0") == 0);
    purefs::fs::filesystem fscore(dm);
    /* Requested filesystem is not registered */
    REQUIRE(fscore.mount("emmc0", "/sys", "vfat") == -ENODEV);
    const auto vfs_vfat = std::make_shared<fs::drivers::filesystem_vfat>();
    REQUIRE(vfs_vfat->mount_count() == 0);
    auto ret = fscore.register_filesystem("vfat", vfs_vfat);
    REQUIRE(ret == 0);
    ret = fscore.mount("emmc0part1", "/sys", "vfat");
    REQUIRE(ret == 0);
    REQUIRE(vfs_vfat->mount_count() == 1);
    REQUIRE(fscore.umount("/ala") == -ENOENT);
    ret = fscore.mount("emmc0part1", "/sys", "vfat");
    REQUIRE(ret == -EBUSY);
    ret = fscore.mount("emmc0part1", "/path", "vfat");
    REQUIRE(ret == -EBUSY);
    ret = fscore.mount("emmc0part2", "/path", "nonexisting_fs");
    REQUIRE(ret == -ENODEV);
    ret = fscore.umount("/sys");
    REQUIRE(ret == 0);
    REQUIRE(vfs_vfat->mount_count() == 0);
    ret = fscore.mount("emmc0part1", "/path", "vfat");
    REQUIRE(ret == 0);
    REQUIRE(vfs_vfat->mount_count() == 1);
    ret = fscore.umount("/path");
    REQUIRE(ret == 0);
}

TEST_CASE("Corefs: Basic API test")
{
    using namespace purefs;
    auto dm   = std::make_shared<blkdev::disk_manager>();
    auto disk = std::make_shared<blkdev::disk_image>(disk_image);
    REQUIRE(disk);
    REQUIRE(dm->register_device(disk, "emmc0") == 0);
    purefs::fs::filesystem fscore(dm);
    /* Requested filesystem is not registered */
    REQUIRE(fscore.mount("emmc0", "/sys", "vfat") == -ENODEV);
    const auto vfs_vfat = std::make_shared<fs::drivers::filesystem_vfat>();
    REQUIRE(vfs_vfat->mount_count() == 0);
    auto ret = fscore.register_filesystem("vfat", vfs_vfat);
    REQUIRE(ret == 0);
    ret = fscore.mount("emmc0part1", "/sys", "vfat");
    REQUIRE(ret == 0);
    {
        struct statvfs ssv;
        ret = fscore.stat_vfs("/sys/", ssv);
        REQUIRE(ret == 0);
    }
    {
        ret = fscore.open("/sys/ala/ma/kota/", 0, 0);
        REQUIRE(ret > 2);
    }
}