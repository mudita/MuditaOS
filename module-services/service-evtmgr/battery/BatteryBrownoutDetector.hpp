// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <hal/battery_charger/AbstractBatteryCharger.hpp>
#include <Service/Service.hpp>
#include <Timers/TimerHandle.hpp>

#include <memory>

namespace sys
{
    class Service;
} // namespace sys

class BatteryBrownoutDetector
{
  public:
    BatteryBrownoutDetector(sys::Service *service, hal::battery::AbstractBatteryCharger &charger);
    void startDetection();

  private:
    void checkBrownout();

    sys::Service *parentService;
    hal::battery::AbstractBatteryCharger &charger;

    bool detectionOngoing     = false;
    unsigned measurementCount = 0;
    sys::TimerHandle measurementTick;
};
