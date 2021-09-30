// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmRecordAction.hpp"
#include <AlarmEventRecord.hpp>
#include <service-time/AlarmServiceAPI.hpp>

namespace alarms
{
    AlarmRecordAction::AlarmRecordAction(sys::Service &service) : service{service}
    {}

    bool AlarmRecordAction::execute(const AlarmEventRecord &record)
    {
        return true;
    }

    bool AlarmRecordAction::turnOff(const AlarmEventRecord &record)
    {
        if (record.rruleText.empty()) {
            auto updatedRecord    = record;
            updatedRecord.enabled = false;
            AlarmServiceAPI::requestUpdateAlarm(&service, updatedRecord);
        }
        return true;
    }
} // namespace alarms
