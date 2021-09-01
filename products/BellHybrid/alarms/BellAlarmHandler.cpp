// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <BellAlarmHandler.hpp>
#include <AlarmHandlerActions.hpp>
#include <service-time/ServiceTime.hpp>

namespace alarms
{
    BellAlarmClockHandler::BellAlarmClockHandler(stm::ServiceTime *serviceTime) : serviceTime(serviceTime)
    {}

    auto BellAlarmClockHandler::handle(const AlarmEventRecord &record) -> bool
    {
        LOG_DEBUG("BellAlarmClockHandler");

        auto result = false;

        if (record.enabled) {
            result = playAlarmSound(record.musicTone);
            result = turnOnFrontlight();
            result = displayAlarmPopup(serviceTime);
            return result;
        }

        return result;
    }

    auto EveningReminderHandler::handle(const AlarmEventRecord &record) -> bool
    {
        LOG_DEBUG("EveningReminderHandler");
        // implement this alarm type handling here
        return true;
    }

} // namespace alarms
