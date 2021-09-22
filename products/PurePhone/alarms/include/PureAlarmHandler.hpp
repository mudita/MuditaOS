// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractAlarmAction.hpp"
#include <service-time/AlarmHandler.hpp>
#include <Service/Service.hpp>

namespace alarms
{
    class PureAlarmHandler : public AlarmHandler
    {
      public:
        using Actions = std::vector<std::unique_ptr<AbstractAlarmAction>>;

        explicit PureAlarmHandler(Actions &&actions = {});
        bool handle(const AlarmEventRecord &record) override;
        bool handleOff(const AlarmEventRecord &record) override;

      private:
        Actions actions;
    };

    class PureAlarmClockHandler : public PureAlarmHandler
    {
      public:
        explicit PureAlarmClockHandler(sys::Service *service);

      private:
        static auto getActions(sys::Service *service) -> Actions;
    };
} // namespace alarms
