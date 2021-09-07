// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryCharger.hpp"

#include <sys/stat.h>
#include <fcntl.h>
#include <EventStore.hpp>
#include <hal/GenericFactory.hpp>
#include <service-evtmgr/battery-level-check/BatteryLevelCheck.hpp>
#include <service-evtmgr/BatteryMessages.hpp>
#include <service-evtmgr/EventManagerCommon.hpp>
#include <magic_enum.hpp>

#include <algorithm>

namespace hal::battery
{
    std::shared_ptr<AbstractBatteryCharger> AbstractBatteryCharger::Factory::create(sys::Service *service)
    {
        return hal::impl::factory<LinuxBatteryCharger, AbstractBatteryCharger>(service);
    }

    namespace
    {
        enum class USBSourceType
        {
            DcdSDP,
            DcdCDP,
            DcdDCP,
        };

        constexpr auto batteryFIFO          = "/tmp/fifoBattKeys";
        constexpr auto fifoFileAccessRights = 0666;
        constexpr auto fifoBuffSize         = 10;
        constexpr auto queueTimeoutTicks    = 100;
        constexpr auto taskDelay            = 50;

        constexpr unsigned fullBattery = 100;

        constexpr auto chargerPlugStateChange = 'p';
        constexpr auto batteryLevelUp         = ']';
        constexpr auto batteryLevelDown       = '[';
        constexpr auto chargerTypeDcdSDP      = 'l';
        constexpr auto chargerTypeDcdCDP      = ';';
        constexpr auto chargerTypeDcdDCP      = '\'';

        xQueueHandle IRQQueueHandle      = nullptr;
        xQueueHandle DCDQueueHandle      = nullptr;
        TaskHandle_t batteryWorkerHandle = nullptr;
        unsigned batteryLevel            = 100;
        bool isPlugged                   = false;

        void batteryWorker(void *parameters)
        {
            mkfifo(batteryFIFO, fifoFileAccessRights);

            // Open FIFO for write only
            int fd = open(batteryFIFO, O_RDONLY | O_NONBLOCK);

            xQueueHandle targetQueueHandle = nullptr;

            while (true) {
                std::uint8_t buff[fifoBuffSize];
                std::int32_t readBytes = read(fd, buff, fifoBuffSize);

                if (readBytes > 0) {
                    std::uint8_t notification = 0;
                    switch (static_cast<char>(buff[0])) {
                    case chargerPlugStateChange:
                        isPlugged         = !isPlugged;
                        targetQueueHandle = IRQQueueHandle;
                        break;
                    case batteryLevelUp:
                        batteryLevel++;
                        targetQueueHandle = IRQQueueHandle;
                        break;
                    case batteryLevelDown:
                        batteryLevel--;
                        targetQueueHandle = IRQQueueHandle;
                        break;
                    case chargerTypeDcdSDP:
                        notification      = static_cast<std::uint8_t>(USBSourceType::DcdSDP);
                        targetQueueHandle = DCDQueueHandle;
                        break;
                    case chargerTypeDcdCDP:
                        notification      = static_cast<std::uint8_t>(USBSourceType::DcdCDP);
                        targetQueueHandle = DCDQueueHandle;
                        break;
                    case chargerTypeDcdDCP:
                        notification      = static_cast<std::uint8_t>(USBSourceType::DcdDCP);
                        targetQueueHandle = DCDQueueHandle;
                        break;
                    default:
                        continue;
                    }
                    xQueueSend(targetQueueHandle, &notification, queueTimeoutTicks);
                }
                vTaskDelay(taskDelay);
            }
        }
    } // namespace

    LinuxBatteryCharger::LinuxBatteryCharger(sys::Service *service) : service{service}
    {}

    void LinuxBatteryCharger::init(xQueueHandle irqQueueHandle, xQueueHandle dcdQueueHandle)
    {
        IRQQueueHandle = irqQueueHandle;
        DCDQueueHandle = dcdQueueHandle;

        xTaskCreate(batteryWorker, "battery", 512, nullptr, 0, &batteryWorkerHandle);
        Store::Battery::modify().level = batteryLevel;
    }

    void LinuxBatteryCharger::deinit()
    {
        IRQQueueHandle = nullptr;
        DCDQueueHandle = nullptr;
        vTaskDelete(batteryWorkerHandle);
    }

    void LinuxBatteryCharger::processStateChangeNotification(std::uint8_t notification)
    {
        if (isPlugged) {
            if (batteryLevel > fullBattery) {
                if (isPlugged && Store::Battery::get().level == fullBattery) {
                    Store::Battery::modify().state = Store::Battery::State::ChargingDone;
                }
            }
            else {
                Store::Battery::modify().state = Store::Battery::State::Charging;
            }
        }
        else {
            Store::Battery::modify().state = Store::Battery::State::Discharging;
        }
        batteryLevel                   = std::clamp(batteryLevel, unsigned{0}, fullBattery);
        Store::Battery::modify().level = batteryLevel;
        battery_level_check::checkBatteryLevel();

        auto message = std::make_shared<sevm::BatteryStatusChangeMessage>();
        service->bus.sendUnicast(std::move(message), service::name::evt_manager);
    }

    void LinuxBatteryCharger::setChargingCurrentLimit(std::uint8_t usbType)
    {
        auto usbTypeStr = magic_enum::enum_name(static_cast<USBSourceType>(usbType));
        LOG_INFO("Setup charging current for usb source type: %s", usbTypeStr.data());
    }

} // namespace hal::battery
