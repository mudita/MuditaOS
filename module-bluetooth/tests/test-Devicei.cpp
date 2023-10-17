// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

TEST_CASE("Given StatefulController when process command successfully then turned on")
{
    auto driver    = std::make_unique<DriverMock>();
    auto processor = std::make_unique<HandlerMock>();
    StatefulController controller{std::move(driver), std::move(processor), InitializerMock};
    controller.turnOn();
    REQUIRE(controller.isOn());

    controller.processCommand(bluetooth::Command(Command::Type::PowerOn));
    REQUIRE(controller.isOn());
}
