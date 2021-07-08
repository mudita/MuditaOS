// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ServiceAlarmImp.hpp"
#include "ServiceAlarm.hpp"

#include <Timers/TimerFactory.hpp>
#include <Service/Service.hpp>

#include <memory>
#include <chrono>

void mcuPeripheralTimerCallback();

void ServiceAlarmImp::Init()
{
    auto dummy_timeout     = std::chrono::milliseconds{3000};
    mcuPeripheralTimerMock = sys::TimerFactory::createSingleShotTimer(
        parent, "MockTImer", dummy_timeout, [this](sys::Timer &) { mcuPeripheralTimerCallback(); });

    alarmCurrent = std::make_shared<AlarmObject>(std::chrono::system_clock::now());
}

sys::MessagePointer ServiceAlarmImp::SetAlarm(sys::Message *message)
{
    sys::MessagePointer responseMsg;
    std::shared_ptr<AlarmObject> alarmRequested = static_cast<AlarmSetMessage *>(message)->getAlarm();

    if (alarmRequested->isOutdated()) {
        responseMsg = std::make_shared<AlarmResponseMessage>(sys::ReturnCodes::Failure, alarmRequested);
        return responseMsg;
    }

    if ((*alarmRequested < *alarmCurrent) || (alarmCurrent->isOutdated())) {
        alarmCurrent  = alarmRequested;
        auto delay_ms = std::chrono::duration_cast<std::chrono::milliseconds>(alarmCurrent->getTimeStamp() -
                                                                              std::chrono::system_clock::now());
        mcuPeripheralTimerMock.restart(delay_ms);
        responseMsg = std::make_shared<AlarmResponseMessage>(sys::ReturnCodes::Success, alarmRequested);
        return responseMsg;
    }
    else {
        responseMsg = std::make_shared<AlarmResponseMessage>(sys::ReturnCodes::Failure, alarmRequested);
        return responseMsg;
    }

    return responseMsg;
}

void ServiceAlarmImp::mcuPeripheralTimerCallback()
{
    auto msg = std::make_shared<AlarmSetMessage>(alarmCurrent);

    parent->bus.sendMulticast(msg, sys::BusChannel::ServiceAlarmNotifications);
}
