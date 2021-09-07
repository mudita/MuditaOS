// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryCharger.hpp"

#include <hal/GenericFactory.hpp>
#include <EventStore.hpp>

namespace hal::battery
{
    BatteryCharger::BatteryCharger(AbstractBatteryCharger::BatteryChargerEvents &eventsHandler)
        : eventsHandler(eventsHandler)
    {}

    void BatteryCharger::init(xQueueHandle queueBatteryHandle, xQueueHandle)
    {
        // Mocking initial state to make system run
        Store::Battery::modify().state = Store::Battery::State::Discharging;
        Store::Battery::modify().level = getBatteryVoltage();
        eventsHandler.onStatusChanged();
    }

    int BatteryCharger::getBatteryVoltage()
    {
        constexpr auto dummyBatteryLevel = 100;
        return dummyBatteryLevel;
    }

    void BatteryCharger::BatteryCharger::deinit()
    {}

    void BatteryCharger::processStateChangeNotification(std::uint8_t notification)
    {}

    void BatteryCharger::setChargingCurrentLimit(std::uint8_t)
    {}

    BaseType_t IRQHandler()
    {
        return 0;
    }

    extern "C"
    {
        void USB_ChargerDetectedCB(std::uint8_t)
        {}
    }

} // namespace hal::battery
