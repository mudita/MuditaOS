// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractAlarmAction.hpp"
#include <service-audio/AudioServiceAPI.hpp>
#include <service-time/AlarmHandler.hpp>
#include <Service/Service.hpp>

namespace alarms
{
    class BellAlarmHandler : public AlarmHandler
    {
      public:
        using Actions = std::vector<std::unique_ptr<AbstractAlarmAction>>;

        explicit BellAlarmHandler(Actions &&actions = {});
        bool handle(const AlarmEventRecord &record) override;
        bool handleOff(const AlarmEventRecord &record) override;

      private:
        Actions actions;
    };

    class BellAlarmClockHandler : public BellAlarmHandler
    {
      public:
        explicit BellAlarmClockHandler(sys::Service *service);

      private:
        static auto getActions(sys::Service *service) -> Actions;
    };

    class PreWakeUpChimeHandler : public BellAlarmHandler
    {
      public:
        explicit PreWakeUpChimeHandler(sys::Service *service);

      private:
        static auto getActions(sys::Service *service) -> Actions;
    };

    class SnoozeChimeHandler : public BellAlarmHandler
    {
      public:
        explicit SnoozeChimeHandler(sys::Service *service);

      private:
        static auto getActions(sys::Service *service) -> Actions;
    };
    class BedtimeReminderHandler : public BellAlarmHandler
    {
      public:
        explicit BedtimeReminderHandler(sys::Service *service);

      private:
        static auto getActions(sys::Service *service) -> Actions;
    };
} // namespace alarms
