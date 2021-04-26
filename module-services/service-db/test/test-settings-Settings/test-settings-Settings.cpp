// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <module-services/service-db/service-db/Settings.hpp>

TEST_CASE("Settings - not initialized")
{
    SECTION("Not initialized settings")
    {
        setting.getValue("lol");
    }

    SECTION("dead initialized settings")
    {
        setting.getValue("lol");
    }
}

TEST_CASE("Settings - not initialized")
{
    settings::Settings setting;
    SECTION("Not initialized settings")
    {
        setting.getValue("lol");
    }

    SECTION("dead initialized settings")
    {
        setting.getValue("lol");
    }
}

TEST_CASE("Settings - initialized")
{
    SECTION("get Value - not exists")
    {}

    SECTION("get Value - exists")
    {}

    SECTION("get Value - different type that expected")
    {}

    SECTION("set value - no value")
    {}

    SECTION("set value - override")
    {}
}
