// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <purefs/fs/filesystem.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/blkdev/disk_image.hpp>

namespace
{
    constexpr auto disk_image = "PurePhone.img";
}

TEST_CASE("Registering and unregistering block device")
{
    using namespace purefs;
    auto dm   = std::make_shared<blkdev::disk_manager>();
    auto disk = std::make_shared<blkdev::disk_image>(disk_image);
    REQUIRE(disk);
    REQUIRE(dm->register_device(disk, "emmc0") == 0);
    purefs::fs::filesystem fscore(dm);
    /* Requested filesystem is not registered */
    REQUIRE(fscore.mount("emmc0", "/sys", "vfat") == -ENODEV);
}
