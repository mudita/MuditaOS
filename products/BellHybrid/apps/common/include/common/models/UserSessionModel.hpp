// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <Timers/TimerHandle.hpp>
#include <Timers/TimerFactory.hpp>

namespace app
{
    inline constexpr auto endOfUserSessionTimeout = std::chrono::seconds{5};

    class AbstractUserSessionModel
    {

      public:
        virtual ~AbstractUserSessionModel() noexcept = default;

        virtual void activateUserSession()            = 0;
        virtual void restartUserSession()             = 0;
        virtual void deactivateUserSessionWithDelay() = 0;
        virtual void setCurrentUserSessionAsHandled() = 0;

        virtual bool isUserSessionActive()        = 0;
        virtual bool isActiveUserSessionHandled() = 0;
    };

    class UserSessionModel : public AbstractUserSessionModel
    {
        enum class SessionState
        {
            Inactive = 0,
            Active,
            EndingWithDelay
        };

      public:
        explicit UserSessionModel(sys::Service *service);

        void activateUserSession() override;
        void restartUserSession() override;
        void deactivateUserSessionWithDelay() override;
        void setCurrentUserSessionAsHandled() override;

        bool isUserSessionActive() override;
        bool isActiveUserSessionHandled() override;

      protected:
        void endOfSessionTimerCallback();

      private:
        SessionState sessionState = SessionState::Inactive;
        sys::Service *service{nullptr};
        sys::TimerHandle endSessionDelayTimer;
        bool isCurrentUserSessionHandled = true;
    };
} // namespace app
