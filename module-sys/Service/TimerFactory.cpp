// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Timers/TimerFactory.hpp>

#include <Timers/SystemTimer.hpp>

namespace sys
{
    TimerHandle TimerFactory::createSingleShotTimer(Service *parent,
                                                    const std::string &name,
                                                    std::chrono::milliseconds interval,
                                                    timer::TimerCallback &&callback)
    {
        auto timer = new timer::SystemTimer(parent, name, interval, timer::Type::SingleShot);
        timer->connect(std::move(callback));
        return TimerHandle{timer};
    }

    TimerHandle TimerFactory::createPeriodicTimer(Service *parent,
                                                  const std::string &name,
                                                  std::chrono::milliseconds interval,
                                                  timer::TimerCallback &&callback)
    {
        auto timer = new timer::SystemTimer(parent, name, interval, timer::Type::Periodic);
        timer->connect(std::move(callback));
        return TimerHandle{timer};
    }
} // namespace sys
