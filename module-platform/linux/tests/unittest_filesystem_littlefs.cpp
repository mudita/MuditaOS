// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <platform/linux/DiskImage.hpp>

#include <purefs/fs/filesystem.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/fs/drivers/filesystem_littlefs.hpp>
#include <sys/statvfs.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>

namespace
{
    constexpr auto disk_image = "lfstest.img";

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
        const auto vfs_lfs = std::make_shared<fs::drivers::filesystem_littlefs>();

        if (fs_core->register_filesystem("littlefs", vfs_lfs) != 0) {
            return {};
        }

        return std::make_pair(std::move(fs_core), std::move(dm));
    }
} // namespace

TEST_CASE("littlefs: Basic mount and functionality")
{
    using namespace purefs;

    auto dm   = std::make_shared<blkdev::disk_manager>();
    auto disk = std::make_shared<blkdev::disk_image>(disk_image);
    REQUIRE(disk);
    REQUIRE(dm->register_device(disk, "emmc0") == 0);

    fs::filesystem fscore(dm);
    const auto vfs_lfs = std::make_shared<fs::drivers::filesystem_littlefs>();
    REQUIRE(vfs_lfs->mount_count() == 0);
    REQUIRE(fscore.register_filesystem("littlefs", vfs_lfs) == 0);

    REQUIRE(fscore.mount("emmc0part0", "/sys", "littlefs") == 0);
    REQUIRE(vfs_lfs->mount_count() == 1);

    SECTION("Mount non-existent partition")
    {
        REQUIRE(fscore.mount("dummy0part0", "/dummy", "littlefs") == -ENXIO);
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
    REQUIRE(vfs_lfs->mount_count() == 0);

    REQUIRE(fscore.mount("emmc0part0", "/path", "littlefs") == 0);
    REQUIRE(vfs_lfs->mount_count() == 1);
    REQUIRE(fscore.umount("/path") == 0);
}

TEST_CASE("littlefs: Read tests")
{
    auto [fs_core, dm] = prepare_filesystem("emmc0");
    REQUIRE(fs_core);
    REQUIRE(fs_core->mount("emmc0part0", "/sys", "littlefs") == 0);

    const auto fd = fs_core->open("/sys/test_read_1.txt", O_RDONLY, 0);
    REQUIRE(fd >= 3);

    static char buf[64];

    SECTION("Simple read")
    {
        REQUIRE(fs_core->read(fd, buf, 8) == 8);
        REQUIRE(memcmp(buf, "01234567", 8) == 0);
    }
    SECTION("Read after seek")
    {
        REQUIRE(fs_core->seek(fd, 4, SEEK_SET) == 4);
        REQUIRE(fs_core->read(fd, buf, 8) == 8);
        REQUIRE(memcmp(buf, "456789AB", 8) == 0);
    }
    SECTION("File fstat")
    {
        struct stat st;
        REQUIRE(fs_core->fstat(fd, st) == 0);
        REQUIRE((st.st_mode & S_IFREG) != 0);
        REQUIRE((st.st_mode & (S_IRUSR | S_IRGRP | S_IROTH)) == (S_IRUSR | S_IRGRP | S_IROTH));
    }

    REQUIRE(fs_core->close(fd) == 0);
    REQUIRE(fs_core->umount("/sys") == 0);
}

TEST_CASE("littlefs: Write tests")
{
    auto [fs_core, dm] = prepare_filesystem("emmc0");
    REQUIRE(fs_core);
    REQUIRE(fs_core->mount("emmc0part0", "/sys", "littlefs") == 0);

    SECTION("Create, write & unlink file")
    {
        static constexpr auto filename = "/sys/test_write_tmp_1.txt";
        auto fd                        = fs_core->open(filename, O_CREAT | O_RDWR, 0);
        REQUIRE(fd >= 3);

        const std::string str = "Hello, littlefs!";
        REQUIRE(fs_core->write(fd, str.c_str(), str.length()) == static_cast<ssize_t>(str.length()));

        // reopen the file to flush write buffers
        REQUIRE(fs_core->close(fd) == 0);
        fd = fs_core->open(filename, O_RDONLY, 0);
        REQUIRE(fd >= 3);

        static char buf[64];
        REQUIRE(fs_core->read(fd, buf, str.length()) == static_cast<ssize_t>(str.length()));
        REQUIRE(memcmp(buf, str.c_str(), str.length()) == 0);

        REQUIRE(fs_core->close(fd) == 0);
        REQUIRE(fs_core->unlink(filename) == 0);
    }

    REQUIRE(fs_core->umount("/sys") == 0);
}

TEST_CASE("littlefs: Read-only filesystem tests")
{
    auto [fs_core, dm] = prepare_filesystem("emmc0");
    REQUIRE(fs_core);
    REQUIRE(fs_core->mount("emmc0part0", "/sys", "littlefs", purefs::fs::mount_flags::read_only) == 0);

    SECTION("Open and stat file with O_RDONLY")
    {
        const auto fd = fs_core->open("/sys/test_read_1.txt", O_RDONLY, 0);
        REQUIRE(fd >= 3);

        struct stat st;
        REQUIRE(fs_core->fstat(fd, st) == 0);
        REQUIRE((st.st_mode & S_IFREG) != 0);
        REQUIRE((st.st_mode & (S_IRUSR | S_IRGRP | S_IROTH)) == (S_IRUSR | S_IRGRP | S_IROTH));
        REQUIRE((st.st_mode & (S_IWUSR | S_IWGRP | S_IWOTH)) == 0);

        REQUIRE(fs_core->close(fd) == 0);
    }
    SECTION("Open file with O_RDWR")
    {
        REQUIRE(fs_core->open("/sys/test_read_1.txt", O_RDWR, 0) == -EACCES);
    }
    SECTION("Create directory")
    {
        REQUIRE(fs_core->mkdir("/sys/tmp_dir", 0) == -EACCES);
    }
    SECTION("Unlink file")
    {
        REQUIRE(fs_core->unlink("/sys/dummy_file.txt") == -EACCES);
    }

    REQUIRE(fs_core->umount("/sys") == 0);
}

TEST_CASE("littlefs: Directory tests")
{
    auto [fs_core, dm] = prepare_filesystem("emmc0");
    REQUIRE(fs_core);
    REQUIRE(fs_core->mount("emmc0part0", "/sys", "littlefs") == 0);

    SECTION("Create directory")
    {
        const std::string path = "/sys/test_mkdir_tmp";
        REQUIRE(fs_core->mkdir(path, 0) == 0);

        SECTION("Attempt to create existing directory")
        {
            REQUIRE(fs_core->mkdir(path, 0) == -EEXIST);
        }
        SECTION("Iterate over directory")
        {
            const std::vector<std::string> filenames = {
                "document.txt", "image.jpg", "ringtone.mp3", "data.csv", "picture.png"};
            std::vector<int> fds;
            std::transform(filenames.begin(), filenames.end(), std::back_inserter(fds), [&](const auto &filename) {
                return fs_core->open(path + "/" + filename, O_CREAT | O_RDWR, 0);
            });
            REQUIRE(std::all_of(fds.begin(), fds.end(), [](auto fd) { return fd >= 3; }));
            const auto dh = fs_core->diropen(path);
            REQUIRE(dh);

            SECTION("Simple iterate")
            {
                std::vector<std::string> dir_filenames;
                int dir_status = 0;
                for (;;) {
                    std::string fn;
                    struct stat st;
                    dir_status = fs_core->dirnext(dh, fn, st);

                    if (dir_status == 0) {
                        dir_filenames.push_back(fn);
                    }
                    else {
                        break;
                    }
                }

                REQUIRE(dir_status == -ENODATA);
                REQUIRE(std::all_of(filenames.begin(), filenames.end(), [&dir_filenames](const auto &fn) {
                    return std::find(dir_filenames.begin(), dir_filenames.end(), fn) != dir_filenames.end();
                }));
            }
            SECTION("Directory reset")
            {
                struct stat st;
                std::string first_fn;
                REQUIRE(fs_core->dirnext(dh, first_fn, st) == 0);

                for (std::string tmp_fn; fs_core->dirnext(dh, tmp_fn, st) == 0;)
                    ;
                REQUIRE(fs_core->dirreset(dh) == 0);

                std::string reset_fn;
                REQUIRE(fs_core->dirnext(dh, reset_fn, st) == 0);
                REQUIRE(reset_fn == first_fn);
            }

            REQUIRE(fs_core->dirclose(dh) == 0);
            for (auto fd : fds) {
                REQUIRE(fs_core->close(fd) == 0);
            }
            for (const auto &filename : filenames) {
                REQUIRE(fs_core->unlink(path + "/" + filename) == 0);
            }
        }

        REQUIRE(fs_core->rmdir(path) == 0);
    }

    REQUIRE(fs_core->umount("/sys") == 0);
}

TEST_CASE("littlefs: Remount RO->RW->RW")
{
    using namespace purefs;
    static constexpr auto filename = "/sys/remount_test.txt";

    auto [fs_core, dm] = prepare_filesystem("emmc0");
    REQUIRE(fs_core);
    REQUIRE(fs_core->mount("emmc0part0", "/sys", "littlefs", fs::mount_flags::read_only) == 0);
    REQUIRE(fs_core->open(filename, O_RDWR | O_CREAT, 0) == -EACCES);

    const std::string wr_str = "remount_test";

    {
        REQUIRE(fs_core->mount("", "/sys", "", fs::mount_flags::remount) == 0);
        const auto fd = fs_core->open(filename, O_RDWR | O_CREAT, 0);
        REQUIRE(fd >= 3);
        REQUIRE(fs_core->write(fd, wr_str.c_str(), wr_str.length()) == static_cast<ssize_t>(wr_str.length()));
        REQUIRE(fs_core->close(fd) == 0);
    }

    {
        REQUIRE(fs_core->mount("", "/sys", "", fs::mount_flags::remount | fs::mount_flags::read_only) == 0);
        const auto fd = fs_core->open(filename, O_RDONLY, 0);
        REQUIRE(fd >= 3);
        char buf[64];
        REQUIRE(fs_core->read(fd, buf, wr_str.length()) == static_cast<ssize_t>(wr_str.length()));
        REQUIRE(memcmp(buf, wr_str.c_str(), wr_str.length()) == 0);
        REQUIRE(fs_core->close(fd) == 0);
    }

    {
        REQUIRE(fs_core->mount("", "/sys", "", fs::mount_flags::remount) == 0);
        REQUIRE(fs_core->unlink(filename) == 0);
    }

    REQUIRE(fs_core->umount("/sys") == 0);
}
