// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <time/AlarmOperations.hpp>

#include <PureAlarmHandler.hpp>

#include <service-db/Settings.hpp>

namespace alarms
{
    std::unique_ptr<IAlarmOperations> AlarmOperationsFactory::create(
        sys::Service *service,
        std::unique_ptr<AbstractAlarmEventsRepository> &&alarmEventsRepo,
        IAlarmOperations::GetCurrentTime getCurrentTimeCallback) const
    {
        auto alarmOperations = std::make_unique<AlarmOperations>(std::move(alarmEventsRepo), getCurrentTimeCallback);
        alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock,
                                                  std::make_shared<alarms::PureAlarmClockHandler>(service));
        return alarmOperations;
    }

    AlarmOperations::AlarmOperations(std::unique_ptr<AbstractAlarmEventsRepository> &&alarmEventsRepo,
                                     GetCurrentTime getCurrentTimeCallback)
        : AlarmOperationsCommon{std::move(alarmEventsRepo), std::move(getCurrentTimeCallback)}
    {}

    void AlarmOperations::minuteUpdated(TimePoint now)
    {
        AlarmOperationsCommon::minuteUpdated(now);
    }
} // namespace alarms
