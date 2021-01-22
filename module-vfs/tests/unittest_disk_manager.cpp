// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <cstdio>
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/blkdev/disk_image.hpp>

namespace
{
    constexpr auto disk_image = "PurePhone.img";
}
TEST_CASE("Registering and unregistering device")
{
    using namespace purefs;
    blkdev::disk_manager dm;
    auto disk = std::make_shared<blkdev::disk_image>(disk_image);
    REQUIRE(disk);
    REQUIRE(dm.register_device(disk, "emmc0") == 0);
    REQUIRE(dm.register_device(disk, "emmc0") == -EEXIST);
    REQUIRE(dm.unregister_device("emmc124") == -ENOENT);
    const auto handle = dm.device_handle("emmc0");
    REQUIRE(handle);
    REQUIRE(dm.unregister_device("emmc0") == 0);
    const auto handle2 = dm.device_handle("emmc0");
    REQUIRE(handle2 == nullptr);
}

TEST_CASE("Parsing and checking partititons")
{
    using namespace purefs;
    blkdev::disk_manager dm;
    auto disk = std::make_shared<blkdev::disk_image>(disk_image);
    REQUIRE(disk);
    REQUIRE(dm.register_device(disk, "emmc0") == 0);
    const auto parts = dm.partitions("emmc0");
    REQUIRE(parts.size() > 1);
    unsigned long prev_start = 0;
    int num{};
    for (const auto &part : parts) {
        REQUIRE(part.physical_number > 0);
        REQUIRE(part.start_sector >= 2048);
        REQUIRE(part.num_sectors > 0);
        REQUIRE(part.start_sector >= prev_start);
        REQUIRE(part.type > 0);
        REQUIRE(part.name == ("emmc0part" + std::to_string(num++)));
        prev_start = part.num_sectors + part.start_sector;
    }
}

TEST_CASE("RW boundary checking")
{
    using namespace purefs;
    blkdev::disk_manager dm;
    auto disk = std::make_shared<blkdev::disk_image>(disk_image);
    REQUIRE(disk);
    REQUIRE(dm.register_device(disk, "emmc0") == 0);
    const auto parts = dm.partitions("emmc0");
    REQUIRE(parts.size() > 1);
    const auto &part1 = parts[0];
    // Read sector by disk name
    const auto sect_size = dm.get_info("emmc0", blkdev::info_type::sector_size);
    REQUIRE(sect_size > 0);
    std::vector<uint8_t> buf1(sect_size), buf2(sect_size);
    REQUIRE(dm.read("emmc0", buf1.data(), part1.start_sector, 1) == 0);
    REQUIRE(dm.read(part1.name, buf2.data(), 0, 1) == 0);
    REQUIRE(buf1 == buf2);
}

TEST_CASE("Null pointer passed to disk manager functions")
{
    using namespace purefs;
    blkdev::disk_manager dm;

    SECTION("Register device function")
    {
        std::shared_ptr<blkdev::disk> disk = nullptr;
        REQUIRE(dm.register_device(disk, "emmc0") == -EINVAL);
    }

    SECTION("Write function")
    {
        REQUIRE(dm.write(static_cast<blkdev::disk_fd>(nullptr),
                         nullptr,
                         static_cast<uint64_t>(0),
                         static_cast<std::size_t>(0)) == -EINVAL);
    }

    SECTION("Read function")
    {
        REQUIRE(dm.read(static_cast<blkdev::disk_fd>(nullptr),
                        nullptr,
                        static_cast<uint64_t>(0),
                        static_cast<std::size_t>(0)) == -EINVAL);
    }

    SECTION("Erase function")
    {
        REQUIRE(dm.erase(static_cast<blkdev::disk_fd>(nullptr), 0, 0) == -EINVAL);
    }

    SECTION("Sync function")
    {
        REQUIRE(dm.sync(static_cast<blkdev::disk_fd>(nullptr)) == -EINVAL);
    }

    SECTION("PM control function")
    {
        REQUIRE(dm.pm_control(static_cast<blkdev::disk_fd>(nullptr), blkdev::pm_state::power_off) == -EINVAL);
    }

    SECTION("Status function")
    {
        REQUIRE(dm.status(static_cast<blkdev::disk_fd>(nullptr)) == blkdev::media_status::error);
    }

    SECTION("Partitions function")
    {
        REQUIRE(dm.partitions(static_cast<blkdev::disk_fd>(nullptr)).empty());
    }

    SECTION("Get info function")
    {
        REQUIRE(dm.get_info(static_cast<blkdev::disk_fd>(nullptr), blkdev::info_type::sector_size) == 0);
    }

    SECTION("Reread partitions function")
    {
        REQUIRE(dm.reread_partitions(static_cast<blkdev::disk_fd>(nullptr)) == -EINVAL);
    }
}

TEST_CASE("Boundary checks for partitions")
{
    using namespace purefs;
    blkdev::disk_manager dm;

    SECTION("Register device function")
    {
        REQUIRE(dm.unregister_device("") == -ENOENT);
    }

    SECTION("Write function")
    {
        REQUIRE(dm.write("", nullptr, static_cast<uint64_t>(0), static_cast<std::size_t>(0)) == -ENOENT);
    }

    SECTION("Read function")
    {
        REQUIRE(dm.read("", nullptr, static_cast<uint64_t>(0), static_cast<std::size_t>(0)) == -ENOENT);
    }

    SECTION("Erase function")
    {
        REQUIRE(dm.erase("", 0, 0) == -ENOENT);
    }

    SECTION("Sync function")
    {
        REQUIRE(dm.sync("") == -ENOENT);
    }

    SECTION("PM control function")
    {
        REQUIRE(dm.pm_control("", blkdev::pm_state::power_off) == -ENOENT);
    }

    SECTION("Status function")
    {
        REQUIRE(dm.status("") == blkdev::media_status::error);
    }

    SECTION("Partitions function")
    {
        REQUIRE(dm.partitions("").empty());
    }

    SECTION("Get info function")
    {
        REQUIRE(dm.get_info("", blkdev::info_type::sector_size) == -ENOENT);
    }

    SECTION("Reread partitions function")
    {
        REQUIRE(dm.reread_partitions("") == -ENOENT);
    }
}

TEST_CASE("Disk sectors out of range for partition")
{
    using namespace purefs;
    blkdev::disk_manager dm;
    auto disk = std::make_shared<blkdev::disk_image>(disk_image);
    REQUIRE(disk);
    REQUIRE(dm.register_device(disk, "emmc1") == 0);
    const auto parts = dm.partitions("emmc1");
    REQUIRE(parts.size() > 1);
    const auto sectors   = dm.get_info("emmc1", blkdev::info_type::sector_count);
    const auto sect_size = dm.get_info("emmc1", blkdev::info_type::sector_size);

    SECTION("Read out of range ")
    {
        std::vector<uint8_t> buf(sect_size);
        SECTION("Pass lba bigger than sectors")
        {
            REQUIRE(dm.read("emmc1", buf.data(), sectors + 1, 1) == -ERANGE);
        }
        SECTION("Pass sum of lba and count bigger than sectors")
        {
            REQUIRE(dm.read("emmc1", buf.data(), sectors - 1, 2) == -ERANGE);
        }
    }

    SECTION("Write out of range")
    {
        std::vector<uint8_t> buf(sect_size);
        SECTION("Pass lba bigger than sectors")
        {
            REQUIRE(dm.write("emmc1", buf.data(), sectors + 1, 1) == -ERANGE);
        }
        SECTION("Pass sum of lba and count bigger than sectors")
        {
            REQUIRE(dm.write("emmc1", buf.data(), sectors - 1, 2) == -ERANGE);
        }
    }

    SECTION("Erase out of range")
    {
        SECTION("Pass lba bigger than sectors")
        {
            REQUIRE(dm.erase("emmc1", sectors + 1, 1) == -ERANGE);
        }
        SECTION("Pass sum of lba and count bigger than sectors")
        {
            REQUIRE(dm.erase("emmc1", sectors - 1, 2) == -ERANGE);
        }
    }
}

TEST_CASE("Test access to /tmp")
{
    // test write
    std::string data = "lolcat";
    {
        std::fstream fs;
        fs.open("/tmp/test.txt", std::ios_base::out);
        REQUIRE(fs.is_open());
        fs << data;
        fs.close();
        REQUIRE(!fs.is_open());
    }

    // test that it's actually written
    {
        std::fstream fs_r;
        fs_r.open("/tmp/test.txt", std::ios_base::in);
        REQUIRE(fs_r.is_open());
        std::string data_check;
        fs_r >> data_check;
        REQUIRE(data == data_check);
    }
}
