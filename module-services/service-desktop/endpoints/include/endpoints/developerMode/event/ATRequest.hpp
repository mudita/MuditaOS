// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <chrono>
#include <service-desktop/DesktopEvent.hpp>

namespace sdesktop::developerMode
{
    class ATResponseEvent : public Event
    {
        std::string command;
        std::chrono::milliseconds timeout;

      public:
        explicit ATResponseEvent(std::string command, std::chrono::milliseconds timeout);

        void setResponse(const std::vector<std::string> &response);

        [[nodiscard]] const auto &getCommand() const noexcept
        {
            return command;
        }
        [[nodiscard]] auto getTimeout() const noexcept
        {
            return timeout;
        }
    };
} // namespace sdesktop::developerMode
