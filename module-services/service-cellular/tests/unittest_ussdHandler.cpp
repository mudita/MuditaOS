// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <module-services/service-cellular/src/ussd/USSDHandler.hpp>

TEST_CASE("ussd handler")
{
    using namespace cellular::ussd;

    USSDHandler handler;
    auto onSendCodeCalled               = 0;
    auto onAbortSessionCalled           = 0;
    auto onOpenPushSessionCalled        = 0;
    auto onRequestOpenPushSessionCalled = 0;
    auto onRequestAbortSessionCalled    = 0;

    auto onTimerStartCalled = 0;
    auto onTimerStopCalled  = 0;

    handler.onSendUssdCode = [&onSendCodeCalled](std::string) -> bool {
        onSendCodeCalled++;
        return true;
    };

    handler.onAbortSession = [&onAbortSessionCalled]([[maybe_unused]] bool ussdSessionTimeout) -> bool {
        onAbortSessionCalled++;
        return true;
    };

    handler.onOpenPushSession = [&onOpenPushSessionCalled]() -> bool {
        onOpenPushSessionCalled++;
        return true;
    };

    handler.onRequestOpenPushSession = [&onRequestOpenPushSessionCalled]() -> bool {
        onRequestOpenPushSessionCalled++;
        return true;
    };

    handler.onRequestAbortSession = [&onRequestAbortSessionCalled]() -> bool {
        onRequestAbortSessionCalled++;
        return true;
    };

    handler.onTimerStart = [&onTimerStartCalled]() { onTimerStartCalled++; };

    handler.onTimerStop = [&onTimerStopCalled]() { onTimerStopCalled++; };

    SECTION("HandleUSSDRequest - ok pull session request")
    {
        handler.handleUSSDRequest(cellular::USSDMessage::RequestType::pullSessionRequest);
        REQUIRE(onTimerStartCalled == 1);
        REQUIRE(onSendCodeCalled == 1);

        REQUIRE(onAbortSessionCalled == 0);
        REQUIRE(onOpenPushSessionCalled == 0);
        REQUIRE(onRequestOpenPushSessionCalled == 0);
        REQUIRE(onTimerStopCalled == 0);
        REQUIRE(onRequestAbortSessionCalled == 0);
    }

    SECTION("HandleUSSDRequest - ok abort session request")
    {
        handler.handleUSSDRequest(cellular::USSDMessage::RequestType::abortSession);

        REQUIRE(onAbortSessionCalled == 1);
        REQUIRE(onRequestOpenPushSessionCalled == 1);
        REQUIRE(onTimerStopCalled == 1);

        REQUIRE(onOpenPushSessionCalled == 0);
        REQUIRE(onTimerStartCalled == 0);
        REQUIRE(onSendCodeCalled == 0);
        REQUIRE(onRequestAbortSessionCalled == 0);
    }

    SECTION("HandleUSSDRequest - ok push session request")
    {
        handler.handleUSSDRequest(cellular::USSDMessage::RequestType::pushSessionRequest);
        REQUIRE(onTimerStopCalled == 1);
        REQUIRE(onOpenPushSessionCalled == 1);

        REQUIRE(onSendCodeCalled == 0);
        REQUIRE(onAbortSessionCalled == 0);
        REQUIRE(onRequestOpenPushSessionCalled == 0);
        REQUIRE(onTimerStartCalled == 0);
        REQUIRE(onRequestAbortSessionCalled == 0);
    }

    SECTION("HandleUSSDRequest - failed abort session request")
    {
        handler.onAbortSession = [&onAbortSessionCalled]([[maybe_unused]] bool ussdSessionTimeout) -> bool {
            onAbortSessionCalled++;
            return false;
        };
        handler.handleUSSDRequest(cellular::USSDMessage::RequestType::abortSession);

        REQUIRE(onAbortSessionCalled == 1);
        REQUIRE(onRequestOpenPushSessionCalled == 0);
        REQUIRE(onRequestAbortSessionCalled == 1);

        REQUIRE(onOpenPushSessionCalled == 0);
        REQUIRE(onTimerStartCalled == 0);
        REQUIRE(onSendCodeCalled == 0);
        REQUIRE(onTimerStopCalled == 0);
    }

    SECTION("setUSSDTimer - pullSessionRequest")
    {
        handler.handleUSSDRequest(cellular::USSDMessage::RequestType::pullSessionRequest);

        REQUIRE(onTimerStartCalled == 1);
        REQUIRE(onTimerStopCalled == 0);

        onTimerStartCalled = 0;
        for (std::uint32_t i = 0; i < pullResponseTimeout; i++) {
            handler.handleUSSDTimer();
        }
        REQUIRE(onTimerStartCalled == 0);
        REQUIRE(onTimerStopCalled == 0);

        handler.handleUSSDTimer();
        REQUIRE(onTimerStopCalled == 1);
    }

    SECTION("setUSSDTimer - abortSession")
    {
        handler.handleUSSDRequest(cellular::USSDMessage::RequestType::abortSession);

        REQUIRE(onTimerStartCalled == 0);
        REQUIRE(onTimerStopCalled == 1);

        handler.handleUSSDTimer();
        REQUIRE(onTimerStopCalled == 2);
    }

    SECTION("setUSSDTimer - pushSession")
    {
        handler.handleUSSDRequest(cellular::USSDMessage::RequestType::abortSession);

        REQUIRE(onTimerStartCalled == 0);
        REQUIRE(onTimerStopCalled == 1);

        handler.handleUSSDTimer();
        REQUIRE(onTimerStopCalled == 2);
    }
}
