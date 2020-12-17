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

TEST_CASE("littlefs: Basic API test")
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
    // List partitions
    for (const auto &part : dm->partitions("emmc0")) {
        std::cout << part.name << " " << part.bootable << std::endl;
    }
    ret = fscore.mount("emmc0part0", "/sys", "littlefs");
    REQUIRE(ret == 0);
    {
        struct statvfs ssv;
        ret = fscore.stat_vfs("/sys/", ssv);
        REQUIRE(ret == 0);
    }
    {
        ret = fscore.open("/sys/ala/ma/kota/", 0, 0);
        REQUIRE(ret == -ENOENT);
        // Simple file test
        int hwnd = fscore.open("/sys/.boot.json", 0, 0);
        REQUIRE(hwnd >= 3);
        std::cout << "File open handle " << hwnd << std::endl;
        struct stat st;
        ret = fscore.fstat(hwnd, st);
        REQUIRE(ret == 0);
        std::cout << "File size " << st.st_size << std::endl;
        char buf[4096]{};
        ret = fscore.read(hwnd, buf, sizeof buf);
        REQUIRE(ret > 0);
        ret = fscore.close(hwnd);
        REQUIRE(ret == 0);
        {
            // Simple directory test
            auto dirhandle = fscore.diropen("/sys");
            REQUIRE(dirhandle);
            REQUIRE(dirhandle->error() == 0);
            for (std::string fnm;;) {
                if (fscore.dirnext(dirhandle, fnm, st) != 0) {
                    break;
                }
                else {
                    std::cout << "name " << fnm << " size " << st.st_size << std::endl;
                }
            }
            fscore.dirclose(dirhandle);
        }
        ret = fscore.umount("/sys");
        REQUIRE(ret == 0);
    }
}
