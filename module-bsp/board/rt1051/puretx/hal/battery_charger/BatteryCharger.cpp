// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryChargerIRQ.hpp"
#include "common/WorkerQueue.hpp"
#include "common/soc_scaler.hpp"

#if ENABLE_CURRENT_MEASUREMENT_SCOPE
#include "CurrentMeasurementScope.hpp"
#endif

#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"

#include <hal/battery_charger/AbstractBatteryCharger.hpp>
#include <bsp/battery_charger/battery_charger.hpp>

#include <magic_enum.hpp>

namespace
{
    hal::battery::AbstractBatteryCharger::ChargingStatus transformChargingState(
        bsp::battery_charger::batteryRetval status)
    {
        using Status   = bsp::battery_charger::batteryRetval;
        using NewState = hal::battery::AbstractBatteryCharger::ChargingStatus;
        switch (status) {
        case Status::ChargerCharging:
            return NewState::Charging;
        case Status::ChargingDone:
            return NewState::ChargingDone;
        case Status::ChargerNotCharging:
            return NewState ::Discharging;
        default:
            return NewState::PluggedNotCharging;
        }
    }

    /// A few constants to make code readability better
    constexpr auto int_b_soc_change =
        static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::SOCOnePercentChange);
    constexpr auto int_b_max_temp = static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::maxTemp);
    constexpr auto int_b_min_temp = static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::minTemp);
    constexpr auto int_b_all      = static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::all);
    constexpr auto int_b_miv_v    = static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::minVAlert);

    constexpr auto int_source_charger =
        static_cast<std::uint8_t>(bsp::battery_charger::topControllerIRQsource::CHGR_INT);

    constexpr auto int_source_fuel_gauge =
        static_cast<std::uint8_t>(bsp::battery_charger::topControllerIRQsource::FG_INT);

    constexpr uint16_t clearStatusTickTime_MS = 10000;

    void clearIrqStatusHandler(TimerHandle_t xTimer)
    {
        if (bsp::battery_charger::getStatusRegister()) {
            bsp::battery_charger::clearFuelGuageIRQ(
                static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::all));
        }
    }
} // namespace

namespace hal::battery
{
    class PureBatteryCharger : public AbstractBatteryCharger
    {
      public:
        struct IrqEvents
        {
            enum class Type
            {
                Controller,
                USBChargerAttached
            };
            Type type;
            std::uint8_t chargerType{};
        };
        using BatteryWorkerQueue = WorkerQueue<IrqEvents>;

        explicit PureBatteryCharger(xQueueHandle irqQueueHandle);
        ~PureBatteryCharger();

        Voltage getBatteryVoltage() const final;
        SOC getSOC() const final;
        ChargingStatus getChargingStatus() const final;

        static BatteryWorkerQueue &getWorkerQueueHandle();

      private:
        static constexpr auto workerStackSize = 1024;

        void setChargingCurrentLimit(std::uint8_t usbType);
        void sendNotification(AbstractBatteryCharger::Events event);
        void checkControllerInterrupts();

        QueueHandle_t notificationChannel;
        TimerHandle_t timerHandle;
        inline static std::unique_ptr<BatteryWorkerQueue> workerQueue;
    };

    PureBatteryCharger::PureBatteryCharger(QueueHandle_t irqQueueHandle)
        : notificationChannel{irqQueueHandle},
          timerHandle{xTimerCreate(
              "clearIrqStatusTimer", pdMS_TO_TICKS(clearStatusTickTime_MS), false, nullptr, clearIrqStatusHandler)}
    {

        workerQueue = std::make_unique<BatteryWorkerQueue>(
            "battery_charger",
            [this](const auto &msg) {
                switch (msg.type) {
                case IrqEvents::Type::Controller: {
                    checkControllerInterrupts();
                } break;

                case IrqEvents::Type::USBChargerAttached: {
                    setChargingCurrentLimit(msg.chargerType);
                } break;
                }
            },
            workerStackSize);

        bsp::battery_charger::init();
        bsp::battery_charger::printFuelGaugeInfo();

#if ENABLE_CURRENT_MEASUREMENT_SCOPE
        CurrentMeasurementScope::start();
#endif
    }
    PureBatteryCharger::~PureBatteryCharger()
    {
        bsp::battery_charger::deinit();
    }
    void PureBatteryCharger::setChargingCurrentLimit(std::uint8_t usbType)
    {
        using namespace bsp::battery_charger;
        switch (static_cast<batteryChargerType>(usbType)) {
        case batteryChargerType::DcdTimeOut:
        case batteryChargerType::DcdUnknownType:
        case batteryChargerType::DcdError:
        case batteryChargerType::DcdSDP:
            LOG_INFO("USB current limit set to 500mA");
            setMaxBusCurrent(USBCurrentLimit::lim500mA);
            break;
        case batteryChargerType::DcdCDP:
        case batteryChargerType::DcdDCP:
            LOG_INFO("USB current limit set to 1000mA");
            setMaxBusCurrent(USBCurrentLimit::lim1000mA);
            break;
        }
    }
    AbstractBatteryCharger::Voltage PureBatteryCharger::getBatteryVoltage() const
    {
        return bsp::battery_charger::getVoltageFilteredMeasurement();
    }
    AbstractBatteryCharger::SOC PureBatteryCharger::getSOC() const
    {
        const auto soc        = bsp::battery_charger::getBatteryLevel();
        const auto scaled_soc = scale_soc(soc);
        if (not scaled_soc) {
            LOG_ERROR("SOC is out of valid range.");
            return 0;
        }
        return *scaled_soc;
    }
    AbstractBatteryCharger::ChargingStatus PureBatteryCharger::getChargingStatus() const
    {
        return transformChargingState(bsp::battery_charger::getChargeStatus());
    }
    PureBatteryCharger::BatteryWorkerQueue &PureBatteryCharger::getWorkerQueueHandle()
    {
        return *workerQueue;
    }
    void PureBatteryCharger::checkControllerInterrupts()
    {
        std::array<std::optional<Events>, 4> events;

        const auto topINT = bsp::battery_charger::getTopControllerINTSource();
        if (topINT & int_source_charger) {
            bsp::battery_charger::clearAllChargerIRQs();
            events[magic_enum::enum_integer(Events::Charger)] = Events::Charger;
        }
        if (topINT & int_source_fuel_gauge) {
            const auto status = bsp::battery_charger::getStatusRegister();
            if (status & int_b_miv_v) {
                bsp::battery_charger::clearFuelGuageIRQ(int_b_miv_v);

                if (bsp::battery_charger::getChargeStatus() == bsp::battery_charger::batteryRetval::ChargerCharging) {
                    if (xTimerIsTimerActive(timerHandle) == pdFALSE) {
                        xTimerStart(timerHandle, 0);
                        LOG_DEBUG("Battery Brownout detected while charging");
                    }
                }
                else {
                    events[magic_enum::enum_integer(Events::Brownout)] = Events::Brownout;
                }
            }
            if (status & int_b_soc_change) {
                bsp::battery_charger::clearFuelGuageIRQ(int_b_soc_change);
                events[magic_enum::enum_integer(Events::SOC)] = Events::SOC;
            }
            if (status & int_b_min_temp || status & int_b_max_temp) {
                bsp::battery_charger::clearFuelGuageIRQ(int_b_min_temp | int_b_max_temp);
                bsp::battery_charger::checkTemperatureRange();
            }
            /// Clear other unsupported IRQ sources just in case
            bsp::battery_charger::clearFuelGuageIRQ(int_b_all);
        }

        /// Send notifications
        for (const auto &event : events) {
            if (event) {
                sendNotification(*event);
            }
        }
    }
    void PureBatteryCharger::sendNotification(AbstractBatteryCharger::Events event)
    {
        xQueueSend(notificationChannel, &event, pdMS_TO_TICKS(100));
    }

    std::unique_ptr<AbstractBatteryCharger> AbstractBatteryCharger::Factory::create(xQueueHandle irqQueueHandle)
    {
        return std::make_unique<PureBatteryCharger>(irqQueueHandle);
    }

    extern "C" void USB_ChargerDetectedCB(std::uint8_t detectedType)
    {
        PureBatteryCharger::getWorkerQueueHandle().post(
            {PureBatteryCharger::IrqEvents::Type::USBChargerAttached, detectedType});
    }

    BaseType_t INTBHandlerIRQ()
    {
        return PureBatteryCharger::getWorkerQueueHandle().post({PureBatteryCharger::IrqEvents::Type::Controller});
    }

    BaseType_t INOKBHandlerIRQ()
    {
        return PureBatteryCharger::getWorkerQueueHandle().post({PureBatteryCharger::IrqEvents::Type::Controller});
    }

} // namespace hal::battery
