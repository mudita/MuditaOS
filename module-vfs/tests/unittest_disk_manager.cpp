// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
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
