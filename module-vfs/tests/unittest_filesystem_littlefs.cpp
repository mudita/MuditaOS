// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <purefs/fs/filesystem.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/blkdev/disk_image.hpp>
#include <purefs/fs/drivers/filesystem_littlefs.hpp>
#include <sys/statvfs.h>
#include <sys/stat.h>

namespace
{
    constexpr auto disk_image = "lfstest.img";
}

TEST_CASE("Littlefs: Basic mount and functionality")
{
    using namespace purefs;
    auto dm   = std::make_shared<blkdev::disk_manager>();
    auto disk = std::make_shared<blkdev::disk_image>(disk_image);
    REQUIRE(disk);
    REQUIRE(dm->register_device(disk, "emmc0") == 0);
    purefs::fs::filesystem fscore(dm);
    const auto vfs_vfat = std::make_shared<fs::drivers::filesystem_littlefs>();
    REQUIRE(vfs_vfat->mount_count() == 0);
    auto ret = fscore.register_filesystem("littlefs", vfs_vfat);
    REQUIRE(ret == 0);
    ret = fscore.mount("emmc0part0", "/sys", "littlefs");
    REQUIRE(ret == 0);
    REQUIRE(vfs_vfat->mount_count() == 1);
    REQUIRE(fscore.umount("/ala") == -ENOENT);
    ret = fscore.mount("emmc0part0", "/sys", "vfat");
    REQUIRE(ret == -EBUSY);
    ret = fscore.mount("emmc0part0", "/path", "vfat");
    REQUIRE(ret == -EBUSY);
    ret = fscore.umount("/sys");
    REQUIRE(ret == 0);
    REQUIRE(vfs_vfat->mount_count() == 0);
    ret = fscore.mount("emmc0part0", "/path", "littlefs");
    REQUIRE(ret == 0);
    REQUIRE(vfs_vfat->mount_count() == 1);
    ret = fscore.umount("/path");
    REQUIRE(ret == 0);
}
