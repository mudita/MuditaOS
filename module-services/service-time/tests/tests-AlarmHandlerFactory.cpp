// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MockAlarmHandler.hpp"

#include <service-time/AlarmHandlerFactory.hpp>

#include <catch2/catch.hpp>

TEST_CASE("AlarmHandlerFactory Tests")
{
    using namespace alarms;

    SECTION("Create AlarmHandlerFactory")
    {
        AlarmHandlerFactory alarmHandlerFactory;
        REQUIRE(alarmHandlerFactory.getHandler(AlarmType::Clock) == nullptr);
    }

    SECTION("Add/get handler")
    {
        AlarmHandlerFactory alarmHandlerFactory;
        auto handler = std::make_shared<MockAlarmHandler>();
        alarmHandlerFactory.addHandler(AlarmType::Clock, handler);

        REQUIRE(alarmHandlerFactory.getHandler(AlarmType::Clock) != nullptr);
        REQUIRE(alarmHandlerFactory.getHandler(AlarmType::Clock) == handler);
        REQUIRE(alarmHandlerFactory.getHandler(AlarmType::Calendar) == nullptr);
    }
}
