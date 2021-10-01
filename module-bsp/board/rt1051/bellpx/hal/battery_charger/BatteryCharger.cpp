// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryCharger.hpp"

#include <hal/GenericFactory.hpp>
#include <EventStore.hpp>
#include <bsp/battery-charger/battery-charger.cpp>
#include <bsp/fuel_gauge/fuel_gauge.hpp>

namespace hal::battery
{
    BatteryCharger::BatteryCharger(AbstractBatteryCharger::BatteryChargerEvents &eventsHandler)
        : eventsHandler(eventsHandler)
    {}

    void BatteryCharger::init(xQueueHandle queueBatteryHandle, xQueueHandle)
    {
        bsp::battery_charger::init(queueBatteryHandle);
        bsp::fuel_gauge::init(queueBatteryHandle);

        Store::Battery::modify().state = Store::Battery::State::Discharging;
        Store::Battery::modify().level = static_cast<unsigned int>(bsp::fuel_gauge::getBatteryLevel());
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

        if (notification == bsp::fuel_gauge::FuelGaugeAlert) {
            Store::Battery::modify().levelState = Store::Battery::LevelState::CriticalNotCharging;
        }

        if (notification == bsp::fuel_gauge::FuelGaugeUpdate) {
            Store::Battery::modify().level = bsp::fuel_gauge::getBatteryLevel();
        }
        eventsHandler.onStatusChanged();
    }

    void BatteryCharger::setChargingCurrentLimit(std::uint8_t)
    {}

    BaseType_t IRQHandler(AbstractBatteryCharger::IRQSource source)
    {
        if (source == AbstractBatteryCharger::IRQSource::Charger) {
            return bsp::battery_charger::CHG_IRQHandler();
        }
        if (source == AbstractBatteryCharger::IRQSource::FuelGauge) {
            return bsp::fuel_gauge::ALRT_IRQHandler();
        }

        return 0;
    }

    extern "C"
    {
        void USB_ChargerDetectedCB(std::uint8_t)
        {}
    }

} // namespace hal::battery
