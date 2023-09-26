// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <evtmgr/user-activity-handler/UserActivityHandler.hpp>
#include <Timers/TimerFactory.hpp>
#include <system/messages/SentinelRegistrationMessage.hpp>
#include <system/Constants.hpp>

namespace evm
{
    namespace
    {
        constexpr auto userActivityTimerTime = std::chrono::seconds(10);
        constexpr auto userActivityCPULevel  = bsp::CpuFrequencyMHz::Level_5;
        constexpr auto sentinelName          = "UserActivity";
    } // namespace

    UserActivityHandler::UserActivityHandler(sys::Service *parent)
        : cpuSentinel(std::make_shared<sys::CpuSentinel>(sentinelName, parent)),
          activityTimer{sys::TimerFactory::createSingleShotTimer(
              parent, "UserActivityTimer", userActivityTimerTime, [this]([[maybe_unused]] sys::Timer &timer) {
                  onUserActivityTimeout();
              })}
    {
        auto sentinelRegistrationMsg = std::make_shared<sys::SentinelRegistrationMessage>(cpuSentinel);
        parent->bus.sendUnicast(std::move(sentinelRegistrationMsg), service::name::system_manager);
    }

    void UserActivityHandler::handleUserInput()
    {
        if (!activityTimer.isActive()) {
            cpuSentinel->HoldMinimumFrequency(userActivityCPULevel);
        }
        activityTimer.restart(userActivityTimerTime);
    }

    void UserActivityHandler::onUserActivityTimeout()
    {
        cpuSentinel->ReleaseMinimumFrequency();
    }

} // namespace evm
