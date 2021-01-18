// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include <purefs/vfs_subsystem.hpp>

#include <filesystem>

TEST_CASE("VFS linux support")
{
    auto vfs = purefs::subsystem::initialize();
    auto err = purefs::subsystem::mount_defaults();
    REQUIRE(err == 0);

    static constexpr auto filenm = "assets/lang/English.json";

    SECTION("std::filesystem") {
        REQUIRE(std::filesystem::file_size(filenm) > 0);
    }

    SECTION("iterators") {
        std::ifstream myfile(filenm);
        REQUIRE (myfile.is_open());

        auto __first = std::istreambuf_iterator<char>(myfile);
        auto __last = std::istreambuf_iterator<char>();
        std::vector<char> testvec(__first, __last);

        testvec.push_back('\0');
        REQUIRE(std::string(testvec.data()).length() > 0);
    }

    REQUIRE(purefs::subsystem::unmount_all() == 0);
}
