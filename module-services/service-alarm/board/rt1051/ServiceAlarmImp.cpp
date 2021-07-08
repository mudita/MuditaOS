// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceAlarmImp.hpp"

#include <Service/Service.hpp>
#include <memory>

void ServiceAlarmImp::Init()
{
    // FSL RTC Init here for sure
}

sys::MessagePointer ServiceAlarmImp::SetAlarm(sys::Message *message)
{
    // FSL RTC setting alarm
    std::shared_ptr<AlarmObject> alarmRequested = static_cast<AlarmSetMessage *>(message)->getAlarm();
    sys::MessagePointer responseMsg = std::make_shared<AlarmResponseMessage>(sys::ReturnCodes::Failure, alarmRequested);
    return responseMsg;
}