// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Service/Timer.hpp>

#include "EinkScreen.hpp"

#include <chrono>
#include <cstdint>
#include <string>

namespace service::eink
{
    class ServiceEink : public sys::Service
    {
      public:
        explicit ServiceEink(const std::string &name, std::string parent = {});

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *response) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;

      private:
        enum class State
        {
            Running,
            Suspended
        };
        void setState(State state) noexcept;
        bool isInState(State state) const noexcept;

        void enterActiveMode();
        void suspend();
        void updateDisplay(const std::uint8_t *frameBuffer, std::uint32_t bufferSize, bool isDeepRefresh);

        sys::MessagePointer handleEinkModeChangedMessage(sys::Message *message);
        sys::MessagePointer handleImageMessage(sys::Message *message);

        EinkScreen screen;
        State currentState;
        sys::Timer powerOffTimer;
    };
} // namespace service::eink
