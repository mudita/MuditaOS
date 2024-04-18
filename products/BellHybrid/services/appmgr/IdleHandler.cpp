// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <appmgr/IdleHandler.hpp>
#include <service-appmgr/Controller.hpp>
#include <Timers/TimerFactory.hpp>
#include <chrono>

namespace
{
    constexpr auto idleReturnTimerTimeout{std::chrono::minutes{3}};
    constexpr auto idleReturnTimerName{"IdleReturn"};
} // namespace

namespace app::manager
{
    IdleHandler::IdleHandler(sys::Service *serv) : serv{serv}
    {
        idleTimer = sys::TimerFactory::createPeriodicTimer(
            serv, idleReturnTimerName, idleReturnTimerTimeout, [this]([[maybe_unused]] sys::Timer &timer) {
                idleTimerCallback();
            });
    }

    void IdleHandler::handleStartIdleTimer([[maybe_unused]] sys::Message *request)
    {
        idleTimer.restart(idleReturnTimerTimeout);
    }

    void IdleHandler::handleRestartIdleTimer([[maybe_unused]] sys::Message *request)
    {
        if (idleTimer.isActive()) {
            idleTimer.restart(idleReturnTimerTimeout);
        }
    }

    void IdleHandler::handleStopIdleTimer([[maybe_unused]] sys::Message *request)
    {
        idleTimer.stop();
    }

    void IdleHandler::idleTimerCallback()
    {
        LOG_INFO("User is idle - going back to home screen");
        idleTimer.stop();
        app::manager::Controller::sendAction(serv, app::manager::actions::Home);
    }
} // namespace app::manager
