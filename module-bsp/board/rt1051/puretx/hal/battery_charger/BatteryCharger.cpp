// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryChargerIRQ.hpp"
#include "WorkerQueue.hpp"
#include "common/soc_scaler.hpp"

#if ENABLE_CURRENT_MEASUREMENT_SCOPE
#include "CurrentMeasurementScope.hpp"
#endif

#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"

#include <hal/battery_charger/AbstractBatteryCharger.hpp>
#include <bsp/battery_charger/battery_charger.hpp>
#include <log/log.hpp>
#include <magic_enum.hpp>

namespace
{
    hal::battery::AbstractBatteryCharger::ChargingStatus transformChargingState(
        bsp::battery_charger::BatteryRetval status)
    {
        using Status   = bsp::battery_charger::BatteryRetval;
        using NewState = hal::battery::AbstractBatteryCharger::ChargingStatus;
        switch (status) {
        case Status::ChargerCharging:
            return NewState::Charging;
        case Status::ChargingDone:
            return NewState::ChargingDone;
        case Status::ChargerNotCharging:
            return NewState::Discharging;
        default:
            return NewState::PluggedNotCharging;
        }
    }

    hal::battery::AbstractBatteryCharger::TemperatureState transformTemperatureState(
        bsp::battery_charger::TemperatureRanges range)
    {
        using Range = bsp::battery_charger::TemperatureRanges;
        using State = hal::battery::AbstractBatteryCharger::TemperatureState;
        switch (range) {
        case Range::Cdeg1to15:
        case Range::Cdeg15to35:
        case Range::Cdeg35to45:
            return State::Normal;
        case Range::Cold:
            return State::TooLow;
        case Range::Hot:
            return State::TooHigh;
        default:
            return State::Unknown;
        }
    }

    /// A few constants to make code readability better
    constexpr auto int_b_soc_change =
        static_cast<std::uint16_t>(bsp::battery_charger::BatteryINTBSource::SOCOnePercentChange);
    constexpr auto int_b_max_temp = static_cast<std::uint16_t>(bsp::battery_charger::BatteryINTBSource::maxTemp);
    constexpr auto int_b_min_temp = static_cast<std::uint16_t>(bsp::battery_charger::BatteryINTBSource::minTemp);
    constexpr auto int_b_all      = static_cast<std::uint16_t>(bsp::battery_charger::BatteryINTBSource::all);
    constexpr auto int_b_miv_v    = static_cast<std::uint16_t>(bsp::battery_charger::BatteryINTBSource::minVAlert);

    constexpr auto int_source_charger =
        static_cast<std::uint8_t>(bsp::battery_charger::TopControllerIRQsource::CHGR_INT);

    constexpr auto int_source_fuel_gauge =
        static_cast<std::uint8_t>(bsp::battery_charger::TopControllerIRQsource::FG_INT);

    constexpr auto irqClearTimerName   = "BatteryIrqClearTimer";
    constexpr auto irqClearTimerTimeMs = 10000;

    void clearIrqStatusHandler([[maybe_unused]] TimerHandle_t xTimer)
    {
        if (bsp::battery_charger::getStatusRegister() != 0) {
            bsp::battery_charger::clearFuelGaugeIRQ(
                static_cast<std::uint16_t>(bsp::battery_charger::BatteryINTBSource::all));
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
                ControllerINTB,
                USBChargerAttached
            };
            Type type;
            std::uint8_t chargerType{};
        };
        using BatteryWorkerQueue = WorkerQueue<IrqEvents>;

        explicit PureBatteryCharger(xQueueHandle irqQueueHandle);
        ~PureBatteryCharger();

        std::optional<Voltage> getBatteryVoltage() const final;
        std::optional<SOC> getSOC() const final;
        ChargingStatus getChargingStatus() const final;
        ChargerPresence getChargerPresence() const final;
        TemperatureState getTemperatureState() const final;

        static BatteryWorkerQueue &getWorkerQueueHandle();

      private:
        static constexpr auto irqWorkerStackSize = 1024 * 2;

        void setChargingCurrentLimit(std::uint8_t usbType);
        void sendNotification(AbstractBatteryCharger::Events event);
        void checkControllerInterrupts();

        QueueHandle_t notificationChannel;
        TimerHandle_t irqClearTimerHandle;
        inline static std::unique_ptr<BatteryWorkerQueue> workerQueue;
    };

    PureBatteryCharger::PureBatteryCharger(QueueHandle_t irqQueueHandle)
        : notificationChannel{irqQueueHandle},
          irqClearTimerHandle{xTimerCreate(
              irqClearTimerName, pdMS_TO_TICKS(irqClearTimerTimeMs), pdFALSE, nullptr, clearIrqStatusHandler)}
    {

        workerQueue = std::make_unique<BatteryWorkerQueue>(
            "battery_charger",
            [this](const auto &msg) {
                switch (msg.type) {
                case IrqEvents::Type::ControllerINTB:
                    checkControllerInterrupts();
                    break;

                case IrqEvents::Type::USBChargerAttached:
                    setChargingCurrentLimit(msg.chargerType);
                    break;

                default:
                    LOG_ERROR("Unhandled IrqEvent %d", static_cast<int>(msg.type));
                    break;
                }
            },
            irqWorkerStackSize);

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

        const auto event = static_cast<BatteryChargerType>(usbType);
        LOG_INFO("USB charging port discovery result: %s", magic_enum::enum_name(event).data());

        switch (event) {
        case BatteryChargerType::DcdTimeOut:
        case BatteryChargerType::DcdUnknownType:
        case BatteryChargerType::DcdError:
        case BatteryChargerType::DcdSDP:
            LOG_INFO("USB current limit set to 500mA");
            setMaxBusCurrent(USBCurrentLimit::lim500mA);
            break;

        case BatteryChargerType::DcdCDP:
        case BatteryChargerType::DcdDCP:
            LOG_INFO("USB current limit set to 1000mA");
            setMaxBusCurrent(USBCurrentLimit::lim1000mA);
            break;
        }
    }

    std::optional<AbstractBatteryCharger::Voltage> PureBatteryCharger::getBatteryVoltage() const
    {
        return bsp::battery_charger::getVoltageFilteredMeasurement();
    }

    std::optional<AbstractBatteryCharger::SOC> PureBatteryCharger::getSOC() const
    {
        const auto soc = bsp::battery_charger::getBatteryLevel();
        if (!soc.has_value()) {
            LOG_ERROR("Cannot read SOC (I2C issue)");
            return std::nullopt;
        }
        bsp::battery_charger::storeBatteryLevelChange(soc.value());
        const auto scaledSoc = scale_soc(soc.value());
        if (!scaledSoc.has_value()) {
            LOG_ERROR("SOC is out of valid range. SOC: %d", soc.value());
            return std::nullopt;
        }
        return scaledSoc;
    }

    AbstractBatteryCharger::TemperatureState PureBatteryCharger::getTemperatureState() const
    {
        return transformTemperatureState(bsp::battery_charger::getTemperatureRange());
    }

    AbstractBatteryCharger::ChargingStatus PureBatteryCharger::getChargingStatus() const
    {
        return transformChargingState(bsp::battery_charger::getChargeStatus());
    }

    AbstractBatteryCharger::ChargerPresence PureBatteryCharger::getChargerPresence() const
    {
        return bsp::battery_charger::isChargerPlugged() ? AbstractBatteryCharger::ChargerPresence::PluggedIn
                                                        : AbstractBatteryCharger::ChargerPresence::Unplugged;
    }

    PureBatteryCharger::BatteryWorkerQueue &PureBatteryCharger::getWorkerQueueHandle()
    {
        return *workerQueue;
    }

    void PureBatteryCharger::checkControllerInterrupts()
    {
        std::vector<Events> eventsToProcess;

        const auto IRQSource = bsp::battery_charger::getTopControllerINTSource();
        if (!IRQSource.has_value()) {
            return;
        }

        if (static_cast<bool>(IRQSource.value() & int_source_charger)) {
            bsp::battery_charger::clearAllChargerIRQs();
            eventsToProcess.push_back(Events::Charger);
        }
        if (static_cast<bool>(IRQSource.value() & int_source_fuel_gauge)) {
            const auto status = bsp::battery_charger::getStatusRegister();
            if (static_cast<bool>(status & int_b_miv_v)) {
                bsp::battery_charger::clearFuelGaugeIRQ(int_b_miv_v);
                if (bsp::battery_charger::getChargeStatus() == bsp::battery_charger::BatteryRetval::ChargerCharging) {
                    if (xTimerIsTimerActive(irqClearTimerHandle) == pdFALSE) {
                        xTimerStart(irqClearTimerHandle, 0);
                        LOG_INFO("Battery brownout detected while charging");
                    }
                }
                else {
                    eventsToProcess.push_back(Events::Brownout);
                }
            }
            if (static_cast<bool>(status & int_b_soc_change)) {
                bsp::battery_charger::clearFuelGaugeIRQ(int_b_soc_change);
                eventsToProcess.push_back(Events::SOC);
            }
            if (static_cast<bool>(status & int_b_min_temp) || static_cast<bool>(status & int_b_max_temp)) {
                bsp::battery_charger::clearFuelGaugeIRQ(int_b_min_temp | int_b_max_temp);
                bsp::battery_charger::checkTemperatureRange();
            }
            // Clear other unsupported IRQ sources just in case
            bsp::battery_charger::clearFuelGaugeIRQ(int_b_all);
        }

        // Send notifications
        for (const auto &event : eventsToProcess) {
            sendNotification(event);
        }
    }

    void PureBatteryCharger::sendNotification(AbstractBatteryCharger::Events event)
    {
        const auto status = xQueueSend(notificationChannel, &event, pdMS_TO_TICKS(100));
        if (status != pdTRUE) {
            LOG_ERROR("Failed to push event to battery charger queue, status %" PRIi32, status);
        }
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
        return PureBatteryCharger::getWorkerQueueHandle().post({PureBatteryCharger::IrqEvents::Type::ControllerINTB});
    }
} // namespace hal::battery
