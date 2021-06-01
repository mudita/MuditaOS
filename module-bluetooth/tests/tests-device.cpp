// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <Device.hpp>

TEST_CASE("Device test")
{
    auto name = "lol";
    auto dev  = Device(name);
    REQUIRE(dev.name == name);
}

TEST_CASE("Devicei test")
{
    auto name = "lol";
    auto dev  = Devicei(name);
    SECTION("name")
    {
        REQUIRE(dev.name == name);
    }

    SECTION("address size of BD size")
    {
        REQUIRE(BD_ADDR_LEN == sizeof dev.address);
    }

    SECTION("compare")
    {
        Devicei rhs, lhs;

        REQUIRE(Devicei() == Devicei());
        REQUIRE(Devicei("lol") == Devicei("xxx"));
        bd_addr_t addr1{1, 2, 3, 4, 5, 6};
        rhs.setAddress(&addr1);
        REQUIRE(!(lhs == rhs));
        lhs.setAddress(&addr1);
        REQUIRE(lhs == rhs);
    }
}
