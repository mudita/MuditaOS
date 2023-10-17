// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <tar/tar.hpp>

constexpr auto test_tar_entries_count = 68;

std::filesystem::path get_test_data(const std::string &name)
{
    const std::string path = __FILE__;
    return std::filesystem::path{path.substr(0, path.rfind('/'))} / name;
}

TEST_CASE("iterator")
{
    auto count{0};
    for ([[maybe_unused]] const auto &entry : tar::iterator(get_test_data("test.tar"))) {
        count++;
    }
    REQUIRE(count == test_tar_entries_count);
}

TEST_CASE("Unpack")
{
    REQUIRE_NOTHROW(tar::unpack(get_test_data("test.tar")));
    REQUIRE_NOTHROW(tar::unpack(get_test_data("test.tar"), "prefix"));
    REQUIRE_THROWS(tar::unpack(""));
}
