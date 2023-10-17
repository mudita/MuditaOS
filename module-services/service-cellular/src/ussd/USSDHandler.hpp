// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-cellular/CellularMessage.hpp>
#include <cstdint>

namespace cellular::ussd
{
    constexpr std::uint32_t pullResponseTimeout = 30;
    constexpr std::uint32_t pullSessionTimeout  = 60;
    constexpr std::uint32_t noTimeout           = 0;

    enum class State
    {
        none,
        pushSession,
        pullRequestSent,
        pullResponseReceived,
        sessionAborted
    };

    class USSDHandler
    {
      public:
        bool handleUSSDRequest(cellular::USSDMessage::RequestType requestType, const std::string &request = "");
        void handleUSSDTimer();
        void externalRequestHandled();
        bool handleURC();
        void setUSSDTimer();
        bool ussdSessionTimeout();

        void setTimerStartCallback(std::function<void(void)> callback)
        {
            onTimerStart = std::move(callback);
        }

        void setTimerGetStateCallback(std::function<bool(void)> callback)
        {
            onTimerGetState = std::move(callback);
        }

        void setTimerStopCallback(std::function<void(void)> callback)
        {
            onTimerStop = std::move(callback);
        }

        std::function<void(void)> onTimerStop;
        std::function<void(void)> onTimerStart;
        std::function<bool(void)> onTimerGetState;
        std::function<bool(void)> onOpenPushSession;
        std::function<bool(bool)> onAbortSession;
        std::function<bool(const std::string &)> onSendUssdCode;
        std::function<void(void)> onRequestAbortSession;
        std::function<void(void)> onRequestOpenPushSession;

      private:
        State ussdState      = State::none;
        std::uint32_t ussdTimeout = 0;
    };
} // namespace cellular::ussd
