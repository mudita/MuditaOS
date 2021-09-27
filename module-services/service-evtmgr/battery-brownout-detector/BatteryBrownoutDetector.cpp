// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryBrownoutDetector.hpp"

#include <service-evtmgr/BatteryMessages.hpp>
#include <system/Constants.hpp>
#include <Timers/TimerFactory.hpp>

namespace
{
    constexpr std::chrono::milliseconds measurementTickTime{1000};
    constexpr auto measurementTickName     = "BrtownoutDetectorTick";
    constexpr unsigned measurementMaxCount = 5;
    constexpr auto brownoutLevelVoltage    = 3600; // mV
} // namespace

BatteryBrownoutDetector::BatteryBrownoutDetector(sys::Service *service,
                                                 std::shared_ptr<hal::battery::AbstractBatteryCharger> charger)
    : parentService(service),
      charger(std::move(charger)), measurementTick{sys::TimerFactory::createSingleShotTimer(
                                       service, measurementTickName, measurementTickTime, [this](sys::Timer &) {
                                           checkBrownout();
                                       })}
{}

void BatteryBrownoutDetector::startDetection()
{
    if (detectionOngoing) {
        return;
    }
    LOG_DEBUG("Battery Brownout detection window start");
    detectionOngoing = true;
    measurementCount = 0;
    checkBrownout();
}

void BatteryBrownoutDetector::checkBrownout()
{
    if (charger->getBatteryVoltage() < brownoutLevelVoltage) {
        LOG_DEBUG("Battery Brownout detected");

        auto messageBrownout = std::make_shared<sevm::BatteryBrownoutMessage>();
        parentService->bus.sendUnicast(std::move(messageBrownout), service::name::system_manager);

        return;
    }

    measurementCount++;
    if (measurementCount <= measurementMaxCount) {
        measurementTick.start();
    }
    else {
        LOG_DEBUG("Battery Brownout detection window finish with negative result");
        detectionOngoing = false;
    }
}
