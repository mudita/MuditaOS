// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <SettingsSerializer.hpp>

bool operator==(const Devicei &lhs, const Devicei &rhs)
{
    return std::memcmp(lhs.address, rhs.address, sizeof lhs.address) == 0 && lhs.name == rhs.name;
}

TEST_CASE("Settings serializer")
{
    auto name = "lol";
    auto dev  = Devicei(name);
    bd_addr_t addr{0x01, 0x02, 0x03, 0, 0, 0};
    dev.setAddress(&addr);

    auto s = SettingsSerializer();

    auto serialized = s.toString({dev});

    SECTION("compare stringified device to same stringified device")
    {
        REQUIRE(s.toString({dev}) == s.toString({dev}));
    }

    SECTION("empty string -results in empty parse")
    {
        REQUIRE(s.fromString("").size() == 0);
    }

    SECTION("non empty string -results in non empty parse")
    {
        REQUIRE(s.fromString(serialized).size() == 1);
    }

    SECTION("vector of devices is the same after serialization and deserialization")
    {
        REQUIRE(std::vector<Devicei>{dev} == s.fromString(serialized));
        REQUIRE(std::vector<Devicei>{dev, dev} == s.fromString(s.toString({dev, dev})));
        REQUIRE(std::memcmp(s.fromString(serialized)[0].address, addr, 6) == 0);
        REQUIRE(s.fromString(serialized)[0].name == name);
    }
}
