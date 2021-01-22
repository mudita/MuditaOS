// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <sys/stat.h>
#include <fcntl.h>
#include <module-utils/common_data/EventStore.hpp>

#include "board.h"
#include "bsp/battery-charger/battery_charger.hpp"

namespace bsp::battery_charger
{
    namespace
    {
        xQueueHandle IRQQueueHandle         = nullptr;
        TaskHandle_t batteryWorkerHandle    = nullptr;
        constexpr auto batteryFIFO          = "/tmp/fifoBattKeys";
        constexpr auto fifoFileAccessRights = 0666;
        constexpr auto fifoBuffSize         = 10;
        constexpr auto queueTimeoutTicks    = 100;
        constexpr auto taskDelay            = 50;

        StateOfCharge battLevel             = 100;
        constexpr StateOfCharge fullBattery = 100;
        bool plugged                        = false;

        constexpr auto chargerPlugStateChange = 'p';
        constexpr auto batteryLevelUp         = ']';
        constexpr auto batteryLevelDown       = '[';

        void battery_worker(void *parameters)
        {
            mkfifo(batteryFIFO, fifoFileAccessRights);

            // Open FIFO for write only
            int fd = open(batteryFIFO, O_RDONLY | O_NONBLOCK);

            while (true) {
                std::uint8_t buff[fifoBuffSize];
                std::int32_t readBytes = read(fd, buff, fifoBuffSize);

                if (readBytes > 0) {
                    std::uint8_t notification = 0;
                    switch (static_cast<char>(buff[0])) {
                    case chargerPlugStateChange:
                        notification = static_cast<std::uint8_t>(batteryIRQSource::INOKB);
                        plugged      = !plugged;
                        break;
                    case batteryLevelUp:
                        notification = static_cast<std::uint8_t>(batteryIRQSource::INTB);
                        if (battLevel < fullBattery)
                            battLevel++;
                        else {
                            // second 100% in a row
                            if (plugged && Store::Battery::get().level == fullBattery) {
                                Store::Battery::modify().state = Store::Battery::State::PluggedNotCharging;
                            }
                        }
                        break;
                    case batteryLevelDown:
                        notification = static_cast<std::uint8_t>(batteryIRQSource::INTB);
                        if (battLevel >= 1)
                            battLevel--;
                        if (plugged && Store::Battery::get().level == fullBattery) {
                            // charging but not 100% anymore
                            Store::Battery::modify().state = Store::Battery::State::Charging;
                        }
                        break;
                    }
                    xQueueSend(IRQQueueHandle, &notification, queueTimeoutTicks);
                }
                vTaskDelay(taskDelay);
            }
        }
    } // namespace

    int init(xQueueHandle queueHandle)
    {
        IRQQueueHandle = queueHandle;
        if (xTaskCreate(battery_worker, "battery", 512, queueHandle, 0, &batteryWorkerHandle) != pdPASS) {
            return 1;
        }
        Store::Battery::modify().level = battLevel;
        return 0;
    }

    void deinit()
    {
        IRQQueueHandle = nullptr;
        vTaskDelete(batteryWorkerHandle);
    }

    StateOfCharge getBatteryLevel()
    {
        Store::Battery::modify().level = battLevel;
        return battLevel;
    }

    bool getChargeStatus()
    {
        bool status = plugged;
        if (status) {
            Store::Battery::modify().state = Store::Battery::State::Charging;
        }
        else {
            Store::Battery::modify().state = Store::Battery::State::Discharging;
        }
        return status;
    }

    // TODO function unused in linux driver, left for compatibility with target driver
    void clearAllIRQs()
    {}
    // TODO function unused in linux driver, left for compatibility with target driver
    void clearFuelGuageIRQ()
    {}

    std::uint16_t getStatusRegister()
    {
        return static_cast<std::uint16_t>(batteryINTBSource::SOCOnePercentChange);
    }

} // namespace bsp::battery_charger
