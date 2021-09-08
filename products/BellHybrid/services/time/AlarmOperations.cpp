// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <time/AlarmOperations.hpp>

namespace alarms
{
    std::unique_ptr<IAlarmOperations> AlarmOperationsFactory::create(
        std::unique_ptr<AbstractAlarmEventsRepository> &&alarmEventsRepo,
        IAlarmOperations::GetCurrentTime getCurrentTimeCallback) const
    {
        return std::make_unique<AlarmOperations>(std::move(alarmEventsRepo), getCurrentTimeCallback);
    }
} // namespace alarms
