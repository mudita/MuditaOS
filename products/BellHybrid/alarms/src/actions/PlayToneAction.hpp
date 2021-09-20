// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractAlarmAction.hpp"
#include <service-db/Settings.hpp>
#include <Timers/TimerHandle.hpp>
#include <Service/Service.hpp>

namespace alarms
{
    class PlayToneAction : public AbstractAlarmAction
    {
      public:
        explicit PlayToneAction(sys::Service &service);
        bool execute() override;
        bool turnOff() override;

      private:
        void spawnTimer(std::chrono::seconds timeout);
        void detachTimer();

        sys::Service &service;
        settings::Settings settings;
        sys::TimerHandle timer;
    };

} // namespace alarms
