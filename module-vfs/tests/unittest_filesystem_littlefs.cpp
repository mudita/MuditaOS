// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <purefs/fs/filesystem.hpp>
#include <purefs/blkdev/disk_manager.hpp>
#include <purefs/blkdev/disk_image.hpp>
#include <purefs/fs/drivers/filesystem_vfat.hpp>
#include <sys/statvfs.h>
#include <sys/stat.h>

TEST_CASE("Littlefs: Basic tests")
{}
