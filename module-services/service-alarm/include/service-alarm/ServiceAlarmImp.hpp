// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ServiceAlarmMessages.hpp"

#include <Timers/TimerHandle.hpp>
#include <Service/Service.hpp>

#include <memory>

class ServiceAlarmImp
{
  public:
    explicit ServiceAlarmImp(sys::Service *parent) : parent(parent)
    {}

    void Init();
    sys::MessagePointer SetAlarm(sys::Message *message);

  private:
    void mcuPeripheralTimerCallback();

    sys::Service *parent = nullptr;
    sys::TimerHandle mcuPeripheralTimerMock;
    std::shared_ptr<AlarmObject> alarmCurrent;
};
