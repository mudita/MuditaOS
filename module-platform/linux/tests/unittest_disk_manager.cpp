// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <catch2/catch.hpp>
#include <purefs/blkdev/disk_manager.hpp>

#include <platform/linux/DiskImage.hpp>

#include "test-setup.hpp"

#include <filesystem>
#include <fstream>

namespace
{
    constexpr auto part_disk_image     = "test_disk.img";
    constexpr auto part_disk_image_ext = "test_disk_ext.img";
    constexpr auto part_disk_image_bad = "test_disk_bad.img";
    constexpr auto eeprom_image        = "test_eeprom.bin";
} // namespace

TEST_CASE("Registering and unregistering device")
{
    using namespace purefs;
    blkdev::disk_manager dm;
    auto disk = std::make_shared<blkdev::disk_image>(::testing::vfs::disk_image);
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
    auto disk = std::make_shared<blkdev::disk_image>(part_disk_image);
    REQUIRE(disk);
    REQUIRE(dm.register_device(disk, "emmc0") == 0);
    const auto parts = dm.partitions("emmc0");
    REQUIRE(parts.size() == 3);
    unsigned long prev_start = 0;
    int num{};
    for (const auto &part : parts) {
        REQUIRE(part.physical_number > 0);
        REQUIRE(part.start_sector > 0);
        REQUIRE(part.num_sectors > 0);
        REQUIRE(part.start_sector >= prev_start);
        REQUIRE(part.type > 0);
        REQUIRE(part.name == ("emmc0part" + std::to_string(num++)));
        prev_start = part.num_sectors + part.start_sector;
    }
}

TEST_CASE("Parsing and checking extended partititons")
{
    using namespace purefs;
    blkdev::disk_manager dm;
    auto disk = std::make_shared<blkdev::disk_image>(part_disk_image_ext);
    REQUIRE(disk);
    REQUIRE(dm.register_device(disk, "emmc0") == 0);
    const auto parts = dm.partitions("emmc0");

    REQUIRE(parts.size() == 7);
    unsigned long prev_start = 0;
    int num{};
    for (const auto &part : parts) {
        REQUIRE(part.physical_number > 0);
        REQUIRE(part.start_sector >= 1);
        REQUIRE(part.num_sectors > 0);
        REQUIRE(part.num_sectors < 0xFF);
        REQUIRE(part.start_sector >= prev_start);
        REQUIRE(part.type > 0);
        REQUIRE(part.name == ("emmc0part" + std::to_string(num++)));
        prev_start = part.num_sectors + part.start_sector;
    }
}

TEST_CASE("Parsing and checking invalid partititons")
{
    using namespace purefs;
    blkdev::disk_manager dm;
    auto disk = std::make_shared<blkdev::disk_image>(part_disk_image_bad);
    REQUIRE(disk);
    REQUIRE(dm.register_device(disk, "emmc0") == 0);
    const auto parts = dm.partitions("emmc0");

    REQUIRE(parts.size() == 0);
}

TEST_CASE("RW boundary checking")
{
    using namespace purefs;
    blkdev::disk_manager dm;
    auto disk = std::make_shared<blkdev::disk_image>(::testing::vfs::disk_image);
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

TEST_CASE("Alternative partitions in the disk manager")
{
    using namespace purefs;
    blkdev::disk_manager dm;
    auto disk = std::make_shared<blkdev::disk_image>(::testing::vfs::disk_image);
    REQUIRE(disk);
    REQUIRE(dm.register_device(disk, "emmc0") == 0);
    const auto sect_size  = dm.get_info("emmc0", blkdev::info_type::sector_size);
    const auto sect_size1 = dm.get_info("emmc0sys1", blkdev::info_type::sector_size);
    REQUIRE(sect_size > 0);
    REQUIRE(sect_size == sect_size1);
    const auto sect_count = dm.get_info("emmc0sys1", blkdev::info_type::sector_count);
    REQUIRE(sect_count == (32 * 1024L * 1024L) / sect_size);
    const auto sect_count1 = dm.get_info("emmc0", blkdev::info_type::sector_count);
    REQUIRE(sect_count1 > sect_count);
    std::vector<char> buf1(sect_size, 0);
    REQUIRE(dm.read("emmc0", buf1.data(), 0, 1) == 0);
    std::vector<char> buf2(sect_size, 0xAA);
    REQUIRE(dm.write("emmc0sys1", buf2.data(), 0, 1) == 0);
    std::fill(std::begin(buf2), std::end(buf2), 0xBB);
    REQUIRE(dm.write("emmc0sys0", buf2.data(), 0, 1) == 0);
    REQUIRE(dm.read("emmc0", buf2.data(), 0, 1) == 0);
    REQUIRE(buf1 == buf2);
    std::fill(std::begin(buf2), std::end(buf2), 0xAA);
    REQUIRE(dm.read("emmc0sys1", buf1.data(), 0, 1) == 0);
    REQUIRE(buf1 == buf2);
    std::fill(std::begin(buf2), std::end(buf2), 0xBB);
    REQUIRE(dm.read("emmc0sys0", buf1.data(), 0, 1) == 0);
    REQUIRE(buf1 == buf2);
}

TEST_CASE("Disk manager EEPROM emulation")
{
    static constexpr auto eeprom_size        = 32768;
    static constexpr auto eeprom_sector_size = 64;

    using namespace purefs;
    std::ofstream ofc(eeprom_image);
    ofc.close();
    std::filesystem::resize_file(eeprom_image, eeprom_size);
    blkdev::disk_manager dm;
    auto disk = std::make_shared<blkdev::disk_image>(eeprom_image, eeprom_sector_size, 1);
    REQUIRE(disk);
    REQUIRE(dm.register_device(disk, "nvrom0", blkdev::flags::no_parts_scan) == 0);
    const auto sect_size  = dm.get_info("nvrom0", blkdev::info_type::sector_size);
    const auto sect_count = dm.get_info("nvrom0", blkdev::info_type::sector_count);
    REQUIRE(sect_size == eeprom_sector_size);
    REQUIRE(sect_count == eeprom_size / sect_size);
    std::vector<char> buf_in1(sect_size, 0xAA);
    std::vector<char> buf_in2(sect_size, 0xBB);
    REQUIRE(dm.write("nvrom0", buf_in1.data(), 0, 1) == 0);
    REQUIRE(dm.write("nvrom0", buf_in2.data(), sect_count - 1, 1) == 0);
    std::vector<char> buf_out1(sect_size);
    std::vector<char> buf_out2(sect_size);
    REQUIRE(dm.read("nvrom0", buf_out1.data(), 0, 1) == 0);
    REQUIRE(dm.read("nvrom0", buf_out2.data(), sect_count - 1, 1) == 0);
    REQUIRE(buf_in1 == buf_out1);
    REQUIRE(buf_in2 == buf_out2);
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
    auto disk = std::make_shared<blkdev::disk_image>(::testing::vfs::disk_image);
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
