// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryCharger.hpp"

#include <hal/GenericFactory.hpp>
#include <EventStore.hpp>
#include <service-evtmgr/battery-level-check/BatteryLevelCheck.hpp>
#include <service-evtmgr/BatteryMessages.hpp>
#include <service-evtmgr/EventManagerCommon.hpp>
namespace hal::battery
{
    std::shared_ptr<AbstractBatteryCharger> AbstractBatteryCharger::Factory::create(sys::Service *service)
    {
        return hal::impl::factory<BatteryCharger, AbstractBatteryCharger>(service);
    }

    BatteryCharger::BatteryCharger(sys::Service *service) : service{service}
    {}

    void BatteryCharger::init(xQueueHandle queueBatteryHandle, xQueueHandle)
    {
        // Mocking initial state to make system run
        Store::Battery::modify().state = Store::Battery::State::Discharging;
        Store::Battery::modify().level = 100;
        auto message                   = std::make_shared<sevm::BatteryStatusChangeMessage>();
        service->bus.sendUnicast(std::move(message), service::name::evt_manager);
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
