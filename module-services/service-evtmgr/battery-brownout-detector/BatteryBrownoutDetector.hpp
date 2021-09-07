// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Service.hpp>
#include <Timers/TimerHandle.hpp>

namespace sys
{
    class Service;
} // namespace sys

class BatteryBrownoutDetector
{
  public:
    BatteryBrownoutDetector(sys::Service *service,
                            std::function<int()> getMeasurementCallback,
                            std::function<void()> positiveResultCallback,
                            std::function<void()> negativeResultCallback);
    void startDetection();

  private:
    void checkBrownout();

    sys::Service *parentService;
    const std::function<int()> getMeasurementCallback;
    const std::function<void()> positiveResultCallback;
    const std::function<void()> negativeResultCallback;
    bool detectionOngoing;
    unsigned measurementCount;
    sys::TimerHandle measurementTick;
};
