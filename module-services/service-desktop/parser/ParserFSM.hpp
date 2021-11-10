// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "MessageHandler.hpp"

#include <Timers/TimerHandle.hpp>

#include <json11.hpp>
#include <magic_enum.hpp>
#include <string>

namespace sdesktop::endpoints
{

    enum class State
    {
        NoMsg,
        ReceivedPartialHeader,
        ReceivedPartialPayload,
    };

    class StateMachine
    {
      public:
        explicit StateMachine(sys::Service *OwnerService);
        void processMessage(std::string &&msg);
        [[nodiscard]] auto getCurrentState() const noexcept -> State
        {
            return state;
        };

        void setState(const State newState) noexcept
        {
            state = newState;
        }

        void setMessageHandler(std::unique_ptr<MessageHandler> handler);

      private:
        std::string receivedMsg;
        State state = State::NoMsg;
        std::string payload;
        std::string header;
        unsigned long payloadLength   = 0;
        sys::Service *OwnerServicePtr = nullptr;
        std::unique_ptr<MessageHandler> messageHandler;
        sys::TimerHandle parserTimer;

        void parseHeader();
        void resetParser();
        void parsePartialHeader();
        void parseNewMessage();
        void parsePartialMessage();
        void parsePayload();
    };

} // namespace sdesktop::endpoints
