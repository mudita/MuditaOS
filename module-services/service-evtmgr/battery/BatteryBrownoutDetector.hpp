// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <hal/battery_charger/AbstractBatteryCharger.hpp>
#include <Service/Service.hpp>
#include <Timers/TimerHandle.hpp>

namespace sys
{
    class Service;
} // namespace sys

class BatteryBrownoutDetector
{
  public:
    struct Thresholds
    {
        units::Voltage shutdown;
        std::uint8_t measurementMaxCount;
    };

    enum class DetectionResult : std::uint8_t
    {
        eventDetected,
        detectionNegative
    };

    using Callback = std::function<void(DetectionResult result)>;

    BatteryBrownoutDetector(sys::Service *service,
                            hal::battery::AbstractBatteryCharger &charger,
                            Thresholds voltage,
                            Callback callback);

    void poll();

  private:
    void tick();
    bool isTimerExpired();
    bool isVoltageThresholdExceeded();
    void brownoutDetected();
    void brownoutNotDetected();
    void clearDetection();
    void triggerDetection();
    hal::battery::AbstractBatteryCharger &charger;

    bool eventDetectionOngoing        = false;
    unsigned measurementBrownoutCount = 0;
    Thresholds voltage;
    Callback callback;
    sys::TimerHandle measurementTick;
};
