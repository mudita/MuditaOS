// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include "btstack_util.h"
#include <command/Command.hpp>
#include <command/DeviceData.hpp>
#include <command/PhoneNumberData.hpp>

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
