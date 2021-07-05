// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <sys/stat.h>
#include <fcntl.h>
#include <EventStore.hpp>

#include "board.h"
#include "bsp/battery-charger/battery_charger.hpp"

namespace bsp::battery_charger
{
    namespace
    {
        xQueueHandle IRQQueueHandle         = nullptr;
        xQueueHandle DCDQueueHandle         = nullptr;
        TaskHandle_t batteryWorkerHandle    = nullptr;
        constexpr auto batteryFIFO          = "/tmp/fifoBattKeys";
        constexpr auto fifoFileAccessRights = 0666;
        constexpr auto fifoBuffSize         = 10;
        constexpr auto queueTimeoutTicks    = 100;
        constexpr auto taskDelay            = 50;

        StateOfCharge battLevel             = 100;
        constexpr StateOfCharge fullBattery = 100;
        bool plugged                        = false;
        topControllerIRQsource IRQSrc       = topControllerIRQsource::CHGR_INT;

        constexpr auto chargerPlugStateChange = 'p';
        constexpr auto batteryLevelUp         = ']';
        constexpr auto batteryLevelDown       = '[';

        constexpr auto chargerTypeDcdSDP = 'l';
        constexpr auto chargerTypeDcdCDP = ';';
        constexpr auto chargerTypeDcdDCP = '\'';

        void battery_worker(void *parameters)
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
                        notification      = static_cast<std::uint8_t>(batteryIRQSource::INTB);
                        IRQSrc            = topControllerIRQsource::CHGR_INT;
                        plugged           = !plugged;
                        targetQueueHandle = IRQQueueHandle;
                        break;
                    case batteryLevelUp:
                        notification = static_cast<std::uint8_t>(batteryIRQSource::INTB);
                        IRQSrc       = topControllerIRQsource::FG_INT;
                        if (battLevel < fullBattery)
                            battLevel++;
                        else {
                            // second 100% in a row
                            if (plugged && Store::Battery::get().level == fullBattery) {
                                Store::Battery::modify().state = Store::Battery::State::ChargingDone;
                            }
                        }
                        targetQueueHandle = IRQQueueHandle;
                        break;
                    case batteryLevelDown:
                        notification = static_cast<std::uint8_t>(batteryIRQSource::INTB);
                        IRQSrc       = topControllerIRQsource::FG_INT;
                        if (battLevel >= 1)
                            battLevel--;
                        if (plugged && Store::Battery::get().level == fullBattery) {
                            // charging but not 100% anymore
                            Store::Battery::modify().state = Store::Battery::State::Charging;
                        }
                        targetQueueHandle = IRQQueueHandle;
                        break;
                    case chargerTypeDcdSDP:
                        notification      = static_cast<std::uint8_t>(batteryChargerType::DcdSDP);
                        targetQueueHandle = DCDQueueHandle;
                        break;
                    case chargerTypeDcdCDP:
                        notification      = static_cast<std::uint8_t>(batteryChargerType::DcdCDP);
                        targetQueueHandle = DCDQueueHandle;
                        break;
                    case chargerTypeDcdDCP:
                        notification      = static_cast<std::uint8_t>(batteryChargerType::DcdDCP);
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

    int init(xQueueHandle irqQueueHandle, xQueueHandle dcdQueueHandle)
    {
        IRQQueueHandle = irqQueueHandle;
        DCDQueueHandle = dcdQueueHandle;

        if (xTaskCreate(battery_worker, "battery", 512, nullptr, 0, &batteryWorkerHandle) != pdPASS) {
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
    void clearAllChargerIRQs()
    {}
    // TODO function unused in linux driver, left for compatibility with target driver
    void clearFuelGuageIRQ(std::uint16_t)
    {}

    std::uint16_t getStatusRegister()
    {
        return static_cast<std::uint16_t>(batteryINTBSource::SOCOnePercentChange);
    }

    std::uint8_t getTopControllerINTSource()
    {
        return static_cast<std::uint8_t>(IRQSrc);
    }

    void checkTemperatureRange()
    {}

    void setUSBCurrentLimit(batteryChargerType chargerType)
    {
        switch (chargerType) {
        case batteryChargerType::DcdTimeOut:
            [[fallthrough]];
        case batteryChargerType::DcdUnknownType:
            [[fallthrough]];
        case batteryChargerType::DcdError:
            [[fallthrough]];
        case batteryChargerType::DcdSDP:
            LOG_INFO("USB current limit set to 500mA");
            break;
        case batteryChargerType::DcdCDP:
            [[fallthrough]];
        case batteryChargerType::DcdDCP:
            LOG_INFO("USB current limit set to 1000mA");
            break;
        }
    }

    void actionIfChargerUnplugged()
    {
        if (Store::Battery::get().state == Store::Battery::State::Discharging) {
            LOG_INFO("USB unplugged");
        }
    }

    int getVoltageFilteredMeasurement()
    {
        constexpr auto nominalVoltage = 4350;
        return nominalVoltage;
    }

    BaseType_t INTB_IRQHandler()
    {
        return 1;
    }

} // namespace bsp::battery_charger
