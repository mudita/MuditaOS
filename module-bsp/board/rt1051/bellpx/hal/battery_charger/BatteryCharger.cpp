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
        bsp::fuel_gauge::init(queueBatteryHandle);
        bsp::battery_charger::init(queueBatteryHandle);

        Store::Battery::modify().state = Store::Battery::State::Discharging;
        lastSOC                        = bsp::fuel_gauge::getBatteryLevel();
        Store::Battery::modify().level = static_cast<unsigned int>(lastSOC);
    }

    int BatteryCharger::getBatteryVoltage()
    {
        return bsp::fuel_gauge::getVoltageFilteredMeasurement();
    }

    void BatteryCharger::BatteryCharger::deinit()
    {}

    void BatteryCharger::processStateChangeNotification(std::uint8_t notification)
    {
        bsp::battery_charger::getChargeStatus();


        if (notification == bsp::fuel_gauge::FuelGaugeUpdate) {
            Store::Battery::modify().level = filterSOC();
        }
        eventsHandler.onStatusChanged();
    }

    void BatteryCharger::setChargingCurrentLimit(std::uint8_t)
    {}

    bsp::fuel_gauge::StateOfCharge BatteryCharger::filterSOC()
    {
        const auto currentSOC = bsp::fuel_gauge::getBatteryLevel();

        /** Due to unknown reasons, fuel gauge sometimes reports SOC raising even if charger is unplugged.
            In order to not confuse the user, SOC reported by fuel gage is filtered based on the last valid SOC
            value. This is not the ideal solution to the problem, but rather a quick&dirty solution which absolutely
           does not fixes the root cause.
        **/
        if (Store::Battery::get().state == Store::Battery::State::Discharging && currentSOC > lastSOC) {
            return lastSOC;
        }
        else {
            return currentSOC;
        }
    }

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
        {
            bsp::battery_charger::enableCharging();
        }
    }

} // namespace hal::battery
