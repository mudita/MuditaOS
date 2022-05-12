// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include "btstack_util.h"
#include <command/Command.hpp>
#include <command/PhoneNumberData.hpp>
#include <command/BatteryLevelData.hpp>

using namespace bluetooth;

TEST_CASE("Command data handling test")
{
    utils::PhoneNumber number("123456789", utils::country::getIdByAlpha2Code("PL"));
    Command::CommandPack receivedPack;

    auto queueSimulator = [](bluetooth::Command::CommandPack *pack, bluetooth::Command::CommandPack *targetPack) {
        memcpy(targetPack, pack, sizeof(bluetooth::Command::CommandPack));
        pack->data.release();
    };

    {
        auto data = std::make_unique<PhoneNumberData>(number);
        Command::CommandPack pack;
        pack.commandType = Command::Type::PowerOn;
        pack.data        = std::move(data);
        queueSimulator(&pack, &receivedPack);
    }

    auto receivedCommand = bluetooth::Command(std::move(receivedPack));
    REQUIRE(number == std::get<utils::PhoneNumber::View>(receivedCommand.getData()));
    REQUIRE(receivedCommand.getType() == bluetooth::Command::PowerOn);
}

TEST_CASE("BatteryLevel test")
{

    SECTION("1/5")
    {
        auto data = BatteryLevel(10);
        REQUIRE(data.getBatteryLevel() == 10);
        REQUIRE(data.getBatteryLevelBars() == 0);
    }
    SECTION("2/5")
    {
        auto data = BatteryLevel(20);
        REQUIRE(data.getBatteryLevel() == 20);
        REQUIRE(data.getBatteryLevelBars() == 1);
    }
    SECTION("3/5")
    {
        auto data = BatteryLevel(100);
        REQUIRE(data.getBatteryLevel() == 100);
        REQUIRE(data.getBatteryLevelBars() == 5);
    }
    SECTION("4/5")
    {
        auto data = BatteryLevel(500);
        REQUIRE(data.getBatteryLevel() == 100);
        REQUIRE(data.getBatteryLevelBars() == 5);
    }
    SECTION("5/5")
    {
        auto data = BatteryLevel(0);
        REQUIRE(data.getBatteryLevel() == 0);
        REQUIRE(data.getBatteryLevelBars() == 0);
    }
}
