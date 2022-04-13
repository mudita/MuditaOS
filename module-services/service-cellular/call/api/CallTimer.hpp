// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <Timers/TimerHandle.hpp>

namespace call::api
{
    class Timer
    {
      public:
        virtual void start()      = 0;
        virtual void stop()       = 0;
        virtual time_t duration() = 0;
        virtual ~Timer()          = default;
    };
}; // namespace call::api

class CallTimer : public call::api::Timer
{
    sys::TimerHandle handle;
    std::time_t startActiveTime{};

  public:
    explicit CallTimer(sys::TimerHandle handle);
    void start() override;
    void stop() override;
    time_t duration() override;
};
