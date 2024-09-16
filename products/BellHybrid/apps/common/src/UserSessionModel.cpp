// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "models/UserSessionModel.hpp"
#include <log/log.hpp>

namespace app
{
    UserSessionModel::UserSessionModel(sys::Service *service) : service{service}
    {
        endSessionDelayTimer = sys::TimerFactory::createPeriodicTimer(
            service, "EndOfUserSessionDelay", endOfUserSessionTimeout, [this](sys::Timer &) {
                endOfSessionTimerCallback();
            });
    }

    void UserSessionModel::activateUserSession()
    {
        if (sessionState == SessionState::Inactive) {
            LOG_INFO("User session began");
            isCurrentUserSessionHandled = false;
        }
        sessionState = SessionState::Active;
        endSessionDelayTimer.stop();
    }

    void UserSessionModel::restartUserSession()
    {
        LOG_INFO("User session began (restarted)");
        isCurrentUserSessionHandled = false;
        sessionState                = SessionState::Active;
        endSessionDelayTimer.stop();
    }

    void UserSessionModel::deactivateUserSessionWithDelay()
    {
        sessionState = SessionState::EndingWithDelay;
        endSessionDelayTimer.restart(endOfUserSessionTimeout);
    }

    void UserSessionModel::setCurrentUserSessionAsHandled()
    {
        isCurrentUserSessionHandled = true;
    }

    bool UserSessionModel::isUserSessionActive()
    {
        return (sessionState != SessionState::Inactive);
    }

    bool UserSessionModel::isActiveUserSessionHandled()
    {
        return isCurrentUserSessionHandled && isUserSessionActive();
    }

    void UserSessionModel::endOfSessionTimerCallback()
    {
        LOG_INFO("User session is ended");
        endSessionDelayTimer.stop();
        sessionState = SessionState::Inactive;
    }
} // namespace app
