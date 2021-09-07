// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryController.hpp"

#include <battery-level-check/BatteryLevelCheck.hpp>

#include <service-evtmgr/BatteryMessages.hpp>
#include <service-evtmgr/Constants.hpp>

using sevm::battery::BatteryController;

BatteryController::BatteryController(sys::Service *service)
    : service(service), charger{std::make_shared<hal::battery::BatteryCharger>(*this)},
      brownoutDetector(service, charger)
{}

void BatteryController::handleChargerNotification(std::uint8_t notification)
{
    charger->setChargingCurrentLimit(notification);
}

void BatteryController::handleBatteryNotification(std::uint8_t notification)
{
    charger->processStateChangeNotification(notification);
}

void BatteryController::init(xQueueHandle queueBatteryHandle, xQueueHandle queueChargerDetect)
{
    charger->init(queueBatteryHandle, queueChargerDetect);
}

void BatteryController::deinit()
{
    charger->deinit();
}

void BatteryController::onBrownout()
{
    brownoutDetector.startDetection();
}

void BatteryController::onStatusChanged()
{
    battery_level_check::checkBatteryLevel();

    auto message = std::make_shared<sevm::BatteryStatusChangeMessage>();
    service->bus.sendUnicast(std::move(message), service::name::evt_manager);
}
