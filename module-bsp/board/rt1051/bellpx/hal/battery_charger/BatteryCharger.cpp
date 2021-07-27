// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryCharger.hpp"

#include <hal/GenericFactory.hpp>
#include <EventStore.hpp>
#include <bsp/battery-charger/battery-charger.cpp>

namespace hal::battery
{
    BatteryCharger::BatteryCharger(AbstractBatteryCharger::BatteryChargerEvents &eventsHandler)
        : eventsHandler(eventsHandler)
    {}

    void BatteryCharger::init(xQueueHandle queueBatteryHandle, xQueueHandle)
    {
        bsp::battery_charger::init(queueBatteryHandle);
        // Mocking initial state to make system run
        Store::Battery::modify().state = Store::Battery::State::Discharging;
        Store::Battery::modify().level = getBatteryVoltage();
    }

    int BatteryCharger::getBatteryVoltage()
    {
        constexpr auto dummyBatteryLevel = 100;
        return dummyBatteryLevel;
    }

    void BatteryCharger::BatteryCharger::deinit()
    {}

    void BatteryCharger::processStateChangeNotification(std::uint8_t notification)
    {
        bsp::battery_charger::getChargeStatus();
    }

    void BatteryCharger::setChargingCurrentLimit(std::uint8_t)
    {}

    BaseType_t IRQHandler()
    {
        return bsp::battery_charger::CHG_IRQHandler();
    }

    extern "C"
    {
        void USB_ChargerDetectedCB(std::uint8_t)
        {}
    }

} // namespace hal::battery
