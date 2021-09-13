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
        auto handleOff(const AlarmEventRecord &record) -> bool;

        static constexpr auto name = "BellAlarmClockHandler";

      private:
        sys::Service *service{};
        std::vector<std::unique_ptr<AbstractAlarmAction>> actions;
    };

    class EveningReminderHandler : public AlarmHandler
    {
      public:
        auto handle(const AlarmEventRecord &record) -> bool;
        auto handleOff(const AlarmEventRecord &record) -> bool;
    };

    class PreWakeUpChimeHandler : public AlarmHandler
    {
      public:
        explicit PreWakeUpChimeHandler(sys::Service *service);
        auto handle(const AlarmEventRecord &record) -> bool;
        auto handleOff(const AlarmEventRecord &record) -> bool;

        static constexpr auto name = "PreWakeUpChimeHandler";

      private:
        sys::Service *service{};
    };

    class PreWakeUpFrontlightHandler : public AlarmHandler
    {
      public:
        explicit PreWakeUpFrontlightHandler(sys::Service *service);
        auto handle(const AlarmEventRecord &record) -> bool;
        auto handleOff(const AlarmEventRecord &record) -> bool;

        static constexpr auto name = "PreWakeUpFrontlightHandler";

      private:
        sys::Service *service{};
    };
} // namespace alarms
