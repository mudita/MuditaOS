// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <platform/linux/DiskImage.hpp>

#include <purefs/fs/filesystem.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/fs/drivers/filesystem_reedgefs.hpp>
#include <purefs/vfs_subsystem.hpp>

#include <purefs/fs/thread_local_cwd.hpp>

#include "test-setup.hpp"

#include <tuple>

#include <sys/statvfs.h>
#include <sys/stat.h>
#include <fcntl.h>

TEST_CASE("reedgefs: Registering and unregistering block device")
{
    using namespace purefs;
    auto dm   = std::make_shared<blkdev::disk_manager>();
    auto disk = std::make_shared<blkdev::disk_image>(::testing::vfs::disk_image);
    REQUIRE(disk);
    REQUIRE(dm->register_device(disk, "emmc0") == 0);
    purefs::fs::filesystem fscore(dm);
    /* Requested filesystem is not registered */
    REQUIRE(fscore.mount("emmc0", "/sys", "reedgefs") == -ENODEV);
    const auto vfs_reedgefs = std::make_shared<fs::drivers::filesystem_reedgefs>();
    REQUIRE(vfs_reedgefs->mount_count() == 0);
    auto ret = fscore.register_filesystem("reedgefs", vfs_reedgefs);
    REQUIRE(ret == 0);
    ret = fscore.mount("emmc0part0", "/sys", "reedgefs");
    REQUIRE(ret == 0);
    REQUIRE(vfs_reedgefs->mount_count() == 1);
    REQUIRE(fscore.umount("/ala") == -ENOENT);
    ret = fscore.mount("emmc0part0", "/sys", "reedgefs");
    REQUIRE(ret == -EBUSY);
    ret = fscore.mount("emmc0part0", "/path", "reedgefs");
    REQUIRE(ret == -EBUSY);
    ret = fscore.mount("emmc0part2", "/path", "nonexisting_fs");
    REQUIRE(ret == -ENODEV);
    ret = fscore.umount("/sys");
    REQUIRE(ret == 0);
    REQUIRE(vfs_reedgefs->mount_count() == 0);
    ret = fscore.mount("emmc0part0", "/path", "reedgefs");
    REQUIRE(ret == 0);
    REQUIRE(vfs_reedgefs->mount_count() == 1);
    ret = fscore.umount("/path");
    REQUIRE(ret == 0);
}

TEST_CASE("reedgefs: Basic API test")
{
    using namespace purefs;
    auto dm   = std::make_shared<blkdev::disk_manager>();
    auto disk = std::make_shared<blkdev::disk_image>(::testing::vfs::disk_image);
    REQUIRE(disk);
    REQUIRE(dm->register_device(disk, "emmc0") == 0);
    purefs::fs::filesystem fscore(dm);
    const auto vfs_reedgefs = std::make_shared<fs::drivers::filesystem_reedgefs>();
    REQUIRE(vfs_reedgefs->mount_count() == 0);
    auto ret = fscore.register_filesystem("reedgefs", vfs_reedgefs);
    REQUIRE(ret == 0);
    // List partitions
    for (const auto &part : dm->partitions("emmc0")) {
        std::cout << part.name << " " << part.bootable << std::endl;
    }
    ret = fscore.mount("emmc0part0", "/sys", "reedgefs");
    REQUIRE(ret == 0);
    {
        struct statvfs ssv;
        ret = fscore.stat_vfs("/sys/", ssv);
        REQUIRE(ret == 0);
    }

    // TODO: switch to image once image is ready
    {
        ret = fscore.open("/sys/ala/ma/kota/", 0, 0);
        REQUIRE(ret == -ENOENT);
        // Simple file test
        // TODO: to remove when image ready
        {
            int hwnd = fscore.open("/sys/.boot.json", O_RDWR | O_CREAT, 0);
            REQUIRE(hwnd >= 3);
            const std::string text = "test";
            REQUIRE(fscore.write(hwnd, text.c_str(), text.size()) == ssize_t(text.length()));
            REQUIRE(fscore.close(hwnd) == 0);
        }
        int hwnd = fscore.open("/sys/.boot.json", 0, 0);
        REQUIRE(hwnd >= 3);
        std::cout << "File open handle " << hwnd << std::endl;
        struct stat st;
        ret = fscore.fstat(hwnd, st);
        REQUIRE(ret == 0);
        std::cout << "File size " << st.st_size << std::endl;
        ret = fscore.stat("/sys/.boot.json", st);
        REQUIRE(ret == 0);
        std::cout << "File size " << st.st_size << std::endl;
        char buf[4096]{};
        ret = fscore.read(hwnd, buf, sizeof buf);
        REQUIRE(ret > 0);
        ret = fscore.close(hwnd);
        REQUIRE(ret == 0);
        {
            // TODO: to remove when image ready
            {
                int hwnd = fscore.open("/sys/.boot2.json", O_RDWR | O_CREAT, 0);
                REQUIRE(hwnd >= 3);
                const std::string text = "test";
                REQUIRE(fscore.write(hwnd, text.c_str(), text.size()) == ssize_t(text.length()));
                REQUIRE(fscore.close(hwnd) == 0);
            }
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
            dirhandle = nullptr;
        }
    }
    REQUIRE(fscore.umount("/sys") == 0);
}

TEST_CASE("reedgefs: Create new file, write, read from it")
{
    using namespace purefs;
    auto dm   = std::make_shared<blkdev::disk_manager>();
    auto disk = std::make_shared<blkdev::disk_image>(::testing::vfs::disk_image);
    REQUIRE(disk);
    REQUIRE(dm->register_device(disk, "emmc0") == 0);
    purefs::fs::filesystem fscore(dm);
    const auto vfs_reedgefs = std::make_shared<fs::drivers::filesystem_reedgefs>();
    REQUIRE(vfs_reedgefs->mount_count() == 0);
    auto ret = fscore.register_filesystem("reedgefs", vfs_reedgefs);
    REQUIRE(ret == 0);
    REQUIRE(fscore.mount("emmc0part0", "/sys", "reedgefs") == 0);

    int hwnd = fscore.open("/sys/test.txt", O_RDWR | O_CREAT, 0660);
    REQUIRE(hwnd >= 3);

    const std::string text = "test";
    fscore.write(hwnd, text.c_str(), text.size());

    REQUIRE(fscore.close(hwnd) == 0);
    SECTION("Read from file")
    {
        int hwnd = fscore.open("/sys/test.txt", O_RDONLY, 0);
        REQUIRE(hwnd >= 3);
        char buf[sizeof(text.c_str())] = {0};
        REQUIRE(fscore.read(hwnd, buf, sizeof(buf)) == 4);
        REQUIRE(strcmp(buf, text.c_str()) == 0);
        fscore.close(hwnd);
        REQUIRE(fscore.umount("/sys") == 0);
    }

    SECTION("Test seek file")
    {
        int hwnd = fscore.open("/sys/test.txt", O_RDONLY, 0);
        REQUIRE(hwnd >= 3);
        char buf[4096]{};
        REQUIRE(fscore.read(hwnd, buf, sizeof(buf)) == 4);
        REQUIRE(fscore.seek(hwnd, 0, SEEK_END) == 4);
        REQUIRE(fscore.read(hwnd, buf, sizeof(buf)) == 0);
        REQUIRE(fscore.seek(hwnd, 0, SEEK_SET) == 0);
        fscore.close(hwnd);
        REQUIRE(fscore.umount("/sys") == 0);
    }
}

TEST_CASE("reedgefs: Mount empty strings")
{
    using namespace purefs;
    auto dm   = std::make_shared<blkdev::disk_manager>();
    auto disk = std::make_shared<blkdev::disk_image>(::testing::vfs::disk_image);
    REQUIRE(disk);
    REQUIRE(dm->register_device(disk, "emmc0") == 0);
    purefs::fs::filesystem fscore(dm);
    const auto vfs_reedgefs = std::make_shared<fs::drivers::filesystem_reedgefs>();
    REQUIRE(vfs_reedgefs->mount_count() == 0);
    auto ret = fscore.register_filesystem("reedgefs", vfs_reedgefs);
    REQUIRE(ret == 0);
    REQUIRE(fscore.mount("", "", "") == -EINVAL);
    REQUIRE(fscore.umount("") == -ENOENT);
}

TEST_CASE("reedgefs: Write to not valid file descriptor")
{
    using namespace purefs;
    auto dm   = std::make_shared<blkdev::disk_manager>();
    auto disk = std::make_shared<blkdev::disk_image>(::testing::vfs::disk_image);
    REQUIRE(disk);
    REQUIRE(dm->register_device(disk, "emmc0") == 0);
    purefs::fs::filesystem fscore(dm);
    const auto vfs_reedgefs = std::make_shared<fs::drivers::filesystem_reedgefs>();
    REQUIRE(vfs_reedgefs->mount_count() == 0);
    auto ret = fscore.register_filesystem("reedgefs", vfs_reedgefs);
    REQUIRE(ret == 0);
    REQUIRE(fscore.mount("emmc0part0", "/sys", "reedgefs") == 0);

    int fd = fscore.open("/sys/.boot.json", O_RDWR | O_CREAT, 0);
    REQUIRE(fd >= 3);
    const auto text = "test";
    REQUIRE(fscore.write(0, text, sizeof(text)) == -EBADF);
    REQUIRE(fscore.write(fd + 1, text, sizeof(text)) == -EBADF);
    REQUIRE(fscore.close(fd) == 0);
    REQUIRE(fscore.umount("/sys") == 0);
}

TEST_CASE("reedgefs: Directory operations")
{
    using namespace purefs;
    auto dm   = std::make_shared<blkdev::disk_manager>();
    auto disk = std::make_shared<blkdev::disk_image>(::testing::vfs::disk_image);
    REQUIRE(disk);
    REQUIRE(dm->register_device(disk, "emmc0") == 0);
    purefs::fs::filesystem fscore(dm);
    const auto vfs_reedgefs = std::make_shared<fs::drivers::filesystem_reedgefs>();
    REQUIRE(vfs_reedgefs->mount_count() == 0);
    auto ret = fscore.register_filesystem("reedgefs", vfs_reedgefs);
    REQUIRE(ret == 0);
    REQUIRE(fscore.mount("emmc0part0", "/sys", "reedgefs") == 0);

    REQUIRE(fscore.mkdir("/sys/current", 0) == 0);

    const auto dirhandle = fscore.diropen("/sys/current");
    REQUIRE(dirhandle);
    REQUIRE(dirhandle->error() == 0);

    SECTION("Null pointer handle dirnext")
    {
        struct stat st;
        std::string fnm;
        REQUIRE(fscore.dirnext(nullptr, fnm, st) == -ENXIO);
        REQUIRE(fscore.dirclose(dirhandle) == 0);
        REQUIRE(fscore.umount("/sys") == 0);
    }

    SECTION("Null pointer handle dirclose")
    {
        REQUIRE(fscore.dirclose(nullptr) == -ENXIO);
        REQUIRE(fscore.dirclose(dirhandle) == 0);
        REQUIRE(fscore.umount("/sys") == 0);
    }

    SECTION("Directory reset")
    {
        struct stat st;
        std::vector<std::tuple<std::string, struct stat>> vec;
        for (std::string fnm;;) {
            if (fscore.dirnext(dirhandle, fnm, st) != 0) {
                break;
            }
            else {
                vec.push_back(std::make_tuple(fnm, st));
                std::cout << "name " << fnm << " size " << st.st_size << std::endl;
            }
        }

        fscore.dirreset(dirhandle);
        int i = 0;
        for (std::string fnm;; i++) {
            if (fscore.dirnext(dirhandle, fnm, st) != 0) {
                break;
            }
            else {
                const auto [fnm_vec, st_vec] = vec[i];
                REQUIRE(fnm == fnm_vec);
                REQUIRE(st_vec.st_size == st.st_size);
                std::cout << "name " << fnm << " size " << st.st_size << std::endl;
            }
        }
        REQUIRE(fscore.dirclose(dirhandle) == 0);
        REQUIRE(fscore.umount("/sys") == 0);
    }
}

TEST_CASE("reedgefs: Read only filesystem")
{
    using namespace purefs;
    auto dm   = std::make_shared<blkdev::disk_manager>();
    auto disk = std::make_shared<blkdev::disk_image>(::testing::vfs::disk_image);
    REQUIRE(disk);
    REQUIRE(dm->register_device(disk, "emmc0") == 0);
    purefs::fs::filesystem fscore(dm);
    const auto vfs_reedgefs = std::make_shared<fs::drivers::filesystem_reedgefs>();
    REQUIRE(vfs_reedgefs->mount_count() == 0);
    auto ret = fscore.register_filesystem("reedgefs", vfs_reedgefs);
    REQUIRE(ret == 0);
    REQUIRE(fscore.mount("emmc0part0", "/sys", "reedgefs", fs::mount_flags::read_only) == 0);
    SECTION("Open file in O_RDWR")
    {
        int hwnd = fscore.open("/sys/rotest.txt", O_RDWR | O_CREAT, 0660);
        REQUIRE(hwnd == -EACCES);
        const std::string text = "test";
        fscore.write(hwnd, text.c_str(), text.size());
    }
    SECTION("Check function which not modify fs")
    {
        struct statvfs ssv;
        ret = fscore.stat_vfs("/sys/", ssv);
        REQUIRE(ret == 0);
    }
    SECTION("Check stat to not set S_IW...")
    {
        struct stat st;
        ret = fscore.stat("/sys", st);
        REQUIRE(ret == 0);
        REQUIRE(st.st_mode & S_IFDIR);
        REQUIRE((st.st_mode & (S_IWGRP | S_IWUSR | S_IWOTH)) == 0);
    }
    REQUIRE(fscore.umount("/sys") == 0);
}

TEST_CASE("reedgefs: Remount filesystem from RO to RW and to RO")
{
    using namespace purefs;
    auto dm   = std::make_shared<blkdev::disk_manager>();
    auto disk = std::make_shared<blkdev::disk_image>(::testing::vfs::disk_image);
    REQUIRE(disk);
    REQUIRE(dm->register_device(disk, "emmc0") == 0);
    auto fscore             = std::make_shared<purefs::fs::filesystem>(dm);
    const auto vfs_reedgefs = std::make_shared<fs::drivers::filesystem_reedgefs>();
    REQUIRE(vfs_reedgefs->mount_count() == 0);
    auto ret = fscore->register_filesystem("reedgefs", vfs_reedgefs);
    REQUIRE(ret == 0);
    REQUIRE(fscore->mount("emmc0part0", "/sys", "reedgefs", fs::mount_flags::read_only) == 0);

    {
        const int hwnd = fscore->open("/sys/remount_test.txt", O_RDWR | O_CREAT, 0660);
        REQUIRE(hwnd == -EACCES);
    }
    REQUIRE(fscore->mount("", "/sys", "", fs::mount_flags::remount) == 0);
    {
        int hwnd = fscore->open("/sys/remount_test4.txt", O_RDWR | O_CREAT, 0660);
        REQUIRE(hwnd > 2);
        const std::string text = "test";
        fscore->write(hwnd, text.c_str(), text.size());
        REQUIRE(fscore->close(hwnd) == 0);
    }
    {
        REQUIRE(fscore->mkdir("/sys/current", 0660) == 0);
        struct stat st;
        ret = fscore->stat("/sys", st);
        REQUIRE(ret == 0);
        REQUIRE(st.st_mode & S_IFDIR);
        REQUIRE(st.st_mode & (S_IWGRP | S_IWUSR | S_IWOTH));
    }
    REQUIRE(fscore->umount("/sys") == 0);
}

// TEST_CASE("reedgefs: Autodetect filesystems")
// {
//     using namespace purefs;
//     auto dm   = std::make_shared<blkdev::disk_manager>();
//     auto disk = std::make_shared<blkdev::disk_image>(::testing::vfs::disk_image);
//     REQUIRE(disk);
//     REQUIRE(dm->register_device(disk, "emmc0") == 0);
//     auto fscore         = std::make_shared<purefs::fs::filesystem>(dm);
//     const auto vfs_reedgefs = std::make_shared<fs::drivers::filesystem_reedgefs>();
//     REQUIRE(vfs_reedgefs->mount_count() == 0);
//     auto ret = fscore->register_filesystem("reedgefs", vfs_reedgefs);
//     REQUIRE(ret == 0);
//     REQUIRE(fscore->mount("emmc0part0", "/sys", "auto") == 0);
//     REQUIRE(fscore->umount("/sys") == 0);
// }
