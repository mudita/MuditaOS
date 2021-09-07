#pragma once

#include <chrono>
#include <service-desktop/DeveloperModeMessage.hpp>

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
