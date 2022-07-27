// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <evtmgr/UserActivityHandler.hpp>
#include <Timers/TimerFactory.hpp>

namespace evm
{
    namespace
    {
        constexpr auto userActivityTimerTime = std::chrono::seconds(2);
        constexpr auto userActivityCPULevel  = bsp::CpuFrequencyMHz::Level_4;
    } // namespace

    UserActivityHandler::UserActivityHandler(std::shared_ptr<sys::CpuSentinel> cpuSentinel, sys::Service *parent)
        : cpuSentinel{std::move(cpuSentinel)},
          activityTimer{sys::TimerFactory::createSingleShotTimer(
              parent, "UserActivityTimer", userActivityTimerTime, [this]([[maybe_unused]] sys::Timer &timer) {
                  onUserActivityTimeout();
              })}
    {}

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
