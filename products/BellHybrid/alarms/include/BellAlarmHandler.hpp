// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractAlarmAction.hpp"
#include <service-time/AlarmHandler.hpp>
#include <Service/Service.hpp>

namespace alarms
{
    class BellAlarmClockHandler : public AlarmHandler
    {
      public:
        explicit BellAlarmClockHandler(sys::Service *service);
        auto handle(const AlarmEventRecord &record) -> bool;

        static constexpr auto name = "BellAlarmClockHandler";

      private:
        sys::Service *service{};
        std::vector<std::unique_ptr<AbstractAlarmAction>> actions;
    };

    class EveningReminderHandler : public AlarmHandler
    {
      public:
        auto handle(const AlarmEventRecord &record) -> bool;
    };
} // namespace alarms
