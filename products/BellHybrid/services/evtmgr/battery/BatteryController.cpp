// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <evtmgr/battery/BatteryController.hpp>

#include <module-utils/EventStore/EventStore.hpp>
#include <service-evtmgr/BatteryMessages.hpp>
#include <system/Constants.hpp>

namespace sevm::battery::bell
{
    namespace
    {
        constexpr int lowVoltageThreshold{3400};
    }

    BatteryController::BatteryController(sys::Service *service, xQueueHandle notificationChannel)
        : battery::BatteryController(service, notificationChannel)
    {}

    void BatteryController::update()
    {
        battery::BatteryController::update();

        const auto voltage = Store::Battery::get().voltage;
        if (voltage < lowVoltageThreshold) {
            sendLowVoltageShutdownMessage();
        }
    }

    void BatteryController::sendLowVoltageShutdownMessage()
    {
        auto message = std::make_shared<sevm::BatteryLowVoltageShutdownMessage>();
        getService()->bus.sendUnicast(std::move(message), ::service::name::system_manager);
    }

} // namespace sevm::battery::bell
