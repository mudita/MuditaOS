// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryBrownoutDetector.hpp"

#include <Timers/TimerFactory.hpp>
#include <utility>

namespace
{
    constexpr std::chrono::milliseconds measurementTickTime{1000};
    constexpr auto measurementTickName = "BrownoutDetectorTick";
} // namespace

BatteryBrownoutDetector::BatteryBrownoutDetector(sys::Service *service,
                                                 hal::battery::AbstractBatteryCharger &charger,
                                                 Thresholds voltage,
                                                 Callback callback)
    : charger{charger}, voltage{voltage}, callback{std::move(callback)},
      measurementTick{sys::TimerFactory::createSingleShotTimer(
          service, measurementTickName, measurementTickTime, [this](auto &) { tick(); })}
{}

bool BatteryBrownoutDetector::isVoltageThresholdExceeded()
{
    return charger.getBatteryVoltage() < voltage.shutdown;
}

void BatteryBrownoutDetector::tick()
{
    if (isVoltageThresholdExceeded()) {
        if (isTimerExpired()) {
            brownoutDetected();
        }
    }
    else {
        brownoutNotDetected();
    }
}

bool BatteryBrownoutDetector::isTimerExpired()
{
    if (++measurementBrownoutCount < voltage.measurementMaxCount) {
        measurementTick.start();
        return false;
    }
    return true;
}

void BatteryBrownoutDetector::brownoutDetected()
{
    callback(DetectionResult::eventDetected);
    clearDetection();
}

void BatteryBrownoutDetector::brownoutNotDetected()
{
    // If count is > 0 it means that detection started but didn't finish
    if (measurementBrownoutCount > 0) {
        callback(DetectionResult::detectionNegative);
    }
    clearDetection();
}

void BatteryBrownoutDetector::poll()
{
    if (eventDetectionOngoing) {
        return;
    }
    if (isVoltageThresholdExceeded()) {
        triggerDetection();
    }
}

void BatteryBrownoutDetector::clearDetection()
{
    measurementBrownoutCount = 0;
    eventDetectionOngoing    = false;
}

void BatteryBrownoutDetector::triggerDetection()
{
    eventDetectionOngoing    = true;
    measurementBrownoutCount = 0;
    measurementTick.start();
}
