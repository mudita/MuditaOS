// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <hal/battery_charger/AbstractBatteryCharger.hpp>

#include <magic_enum.hpp>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace hal::battery
{
    namespace
    {
        constexpr auto batteryFIFO          = "/tmp/fifoBattKeys";
        constexpr auto fifoFileAccessRights = 0666;
        constexpr auto fifoBuffSize         = 10;
        constexpr auto queueTimeoutTicks    = 100;
        constexpr auto taskDelay            = 50;

        constexpr auto dummyBatteryVoltageLevel = 3700;

        constexpr auto chargerPlugStateChange = 'p';
        constexpr auto batteryLevelUp         = ']';
        constexpr auto batteryLevelDown       = '[';
        constexpr auto chargerTypeDcdSDP      = 'l';
        constexpr auto chargerTypeDcdCDP      = ';';
        constexpr auto chargerTypeDcdDCP      = '\'';

    } // namespace

    class BatteryCharger : public AbstractBatteryCharger
    {
      public:
        explicit BatteryCharger(xQueueHandle irqQueueHandle);
        ~BatteryCharger();

        Voltage getBatteryVoltage() const final;
        SOC getSOC() const final;
        ChargingStatus getChargingStatus() const final;

      private:
        void worker();

        xQueueHandle notificationChannel = nullptr;
        TaskHandle_t batteryWorkerHandle = nullptr;
        unsigned batteryLevel            = 100;
        bool isPlugged                   = false;
        bool shouldRun                   = true;
    };

    BatteryCharger::BatteryCharger(xQueueHandle irqQueueHandle)
    {
        notificationChannel = irqQueueHandle;

        xTaskCreate(
            [](void *pvp) {
                BatteryCharger *inst = static_cast<BatteryCharger *>(pvp);
                inst->worker();
            },
            "battery",
            512,
            this,
            0,
            &batteryWorkerHandle);
    }

    BatteryCharger::~BatteryCharger()
    {
        shouldRun = false;
        /// The worker operates in taskDelay intervals. Give it at least taskDelay to handle the close procedure.
        vTaskDelay(taskDelay * 2);
    }

    AbstractBatteryCharger::Voltage BatteryCharger::getBatteryVoltage() const
    {
        return dummyBatteryVoltageLevel;
    }

    AbstractBatteryCharger::SOC BatteryCharger::getSOC() const
    {
        return batteryLevel;
    }
    AbstractBatteryCharger::ChargingStatus BatteryCharger::getChargingStatus() const
    {
        if (isPlugged && batteryLevel >= 100) {
            return ChargingStatus::ChargingDone;
        }
        else if (isPlugged && batteryLevel < 100) {
            return ChargingStatus::Charging;
        }
        else {
            return ChargingStatus::Discharging;
        }
    }

    void BatteryCharger::worker()
    {
        mkfifo(batteryFIFO, fifoFileAccessRights);

        // Open FIFO for write only
        int fd = open(batteryFIFO, O_RDONLY | O_NONBLOCK);

        while (shouldRun) {
            std::uint8_t buff[fifoBuffSize];
            std::int32_t readBytes = read(fd, buff, fifoBuffSize);

            if (readBytes > 0) {
                Events evt{};
                switch (static_cast<char>(buff[0])) {
                case chargerPlugStateChange:
                    isPlugged = !isPlugged;
                    evt       = Events::Charger;
                    break;
                case batteryLevelUp:
                    batteryLevel++;
                    evt = Events::SOC;
                    break;
                case batteryLevelDown:
                    batteryLevel--;
                    evt = Events::SOC;
                    break;
                case chargerTypeDcdSDP:
                case chargerTypeDcdCDP:
                case chargerTypeDcdDCP:
                    evt = Events::Charger;
                    break;
                default:
                    continue;
                }
                xQueueSend(notificationChannel, &evt, queueTimeoutTicks);
            }
            vTaskDelay(taskDelay);
        }
        close(fd);
        vTaskDelete(nullptr);
    }

    std::unique_ptr<AbstractBatteryCharger> AbstractBatteryCharger::Factory::create(xQueueHandle irqQueueHandle)
    {
        return std::make_unique<BatteryCharger>(irqQueueHandle);
    }

} // namespace hal::battery
