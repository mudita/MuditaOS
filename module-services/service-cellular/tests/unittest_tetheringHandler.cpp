// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <module-services/service-cellular/src/TetheringHandler.hpp>
#include <module-cellular/at/cmd/QCFGUsbnet.hpp>

TEST_CASE("SMSSendHandler functionality")
{
    SECTION("RNDIS enabled")
    {
        auto handler             = cellular::service::TetheringHandler();
        handler.onIsRndisEnabled = []() -> at::qcfg::usbnet::Net { return at::qcfg::usbnet::Net::RNDIS; };

        auto result = handler.isRndisEnabled();
        REQUIRE(result == true);
    }

    SECTION("RNDIS disabled")
    {

        auto handler             = cellular::service::TetheringHandler();
        handler.onIsRndisEnabled = []() -> at::qcfg::usbnet::Net { return at::qcfg::usbnet::Net::ECM; };
        auto result              = handler.isRndisEnabled();

        REQUIRE(result == false);
    }

    SECTION("configure RNDIS enabled")
    {
        auto handler                = cellular::service::TetheringHandler();
        auto onIsRndisEnabledCalled = 0;
        handler.onIsRndisEnabled    = [&onIsRndisEnabledCalled]() -> at::qcfg::usbnet::Net {
            onIsRndisEnabledCalled++;
            return at::qcfg::usbnet::Net::RNDIS;
        };
        auto result = handler.configure();
        REQUIRE(result == true);
        REQUIRE(onIsRndisEnabledCalled == 1);
    }

    SECTION("configure RNDIS disabled")
    {
        auto handler                = cellular::service::TetheringHandler();
        auto onIsRndisEnabledCalled = 0;
        auto onEnableRndisCalled    = 0;
        handler.onIsRndisEnabled    = [&onIsRndisEnabledCalled]() -> at::qcfg::usbnet::Net {
            onIsRndisEnabledCalled++;
            return at::qcfg::usbnet::Net::ECM;
        };

        handler.onEnableRndis = [&onEnableRndisCalled]() -> bool {
            onEnableRndisCalled++;
            return true;
        };

        auto result = handler.configure();
        REQUIRE(result == false);
        REQUIRE(onIsRndisEnabledCalled == 1);
        REQUIRE(onEnableRndisCalled == 1);
    }

    SECTION("enable tethering")
    {
        auto handler                = cellular::service::TetheringHandler();
        auto onDisableUrcCalled     = 0;
        auto onSetPassthroughCalled = 0;
        handler.onDisableUrc        = [&onDisableUrcCalled]() -> bool {
            onDisableUrcCalled++;
            return true;
        };

        handler.onSetPassthrough = [&onSetPassthroughCalled](bool) { onSetPassthroughCalled++; };

        auto result = handler.enable();
        REQUIRE(result == true);
        REQUIRE(onDisableUrcCalled == 1);
        REQUIRE(onSetPassthroughCalled == 1);
    }

    SECTION("disable tethering")
    {
        auto handler                 = cellular::service::TetheringHandler();
        auto onEnableUrcCalled       = 0;
        auto onReadAllMessagesCalled = 0;
        auto onSetPassthroughCalled  = 0;
        handler.onEnableUrc          = [&onEnableUrcCalled]() -> bool {
            onEnableUrcCalled++;
            return true;
        };

        handler.onReadMessages = [&onReadAllMessagesCalled]() -> bool {
            onReadAllMessagesCalled++;
            return true;
        };

        handler.onSetPassthrough = [&onSetPassthroughCalled](bool) { onSetPassthroughCalled++; };

        auto result = handler.disable();
        REQUIRE(result == true);
        REQUIRE(onEnableUrcCalled == 1);
        REQUIRE(onSetPassthroughCalled == 1);
    }
}
