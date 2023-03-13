// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryChargerIRQ.hpp"
#include "WorkerQueue.hpp"
#include "common/soc_scaler.hpp"

#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"

#include <hal/battery_charger/AbstractBatteryCharger.hpp>
#include <devices/power/MP2639B.hpp>
#include <devices/power/CW2015.hpp>
#include <board/BoardDefinitions.hpp>
#include <log/log.hpp>

namespace
{
    using namespace bsp::devices::power;

    constexpr auto reinit_poll_time = pdMS_TO_TICKS(5 * 60 * 1000); /// 5min
    constexpr auto default_timeout  = pdMS_TO_TICKS(100);

    constexpr auto i2c_baudrate = static_cast<std::uint32_t>(BoardDefinitions::BELL_FUELGAUGE_I2C_BAUDRATE);
    constexpr auto i2c_instance = static_cast<drivers::I2CInstances>(BoardDefinitions::BELL_FUELGAUGE_I2C);
    constexpr drivers::DriverI2CParams i2c_params{i2c_baudrate};

    constexpr auto fuel_gauge_irq_pin = static_cast<std::uint32_t>(BoardDefinitions::BELL_FUELGAUGE_ALRT_PIN);
    constexpr auto fuel_gauge_irq_gpio_instance =
        static_cast<drivers::GPIOInstances>(BoardDefinitions::BELL_FUELGAUGE_ALRT_GPIO);

    constexpr auto charger_irq_pin_acok  = static_cast<std::uint32_t>(BoardDefinitions::BELL_BATTERY_CHARGER_ACOK_PIN);
    constexpr auto charger_irq_pin_mode  = static_cast<std::uint32_t>(BoardDefinitions::BELL_BATTERY_CHARGER_CHGEN_PIN);
    constexpr auto charger_irq_pin_chgok = static_cast<std::uint32_t>(BoardDefinitions::BELL_BATTERY_CHARGER_CHGOK_PIN);

    constexpr auto charger_irq_gpio = static_cast<drivers::GPIOInstances>(BoardDefinitions::BELL_BATTERY_CHARGER_GPIO);
    constexpr auto charger_irq_gpio_chgok =
        static_cast<drivers::GPIOInstances>(BoardDefinitions::BELL_BATTERY_CHARGER_CHGOK_GPIO);

} // namespace

namespace hal::battery
{
    class BellBatteryCharger : public AbstractBatteryCharger
    {
      public:
        enum class IrqEvents : std::uint8_t
        {
            Charger,
            FuelGauge
        };
        using BatteryWorkerQueue = WorkerQueue<IrqEvents>;

        explicit BellBatteryCharger(xQueueHandle irqQueueHandle);
        ~BellBatteryCharger();

        Voltage getBatteryVoltage() const final;
        std::optional<SOC> getSOC() const final;
        ChargingStatus getChargingStatus() const final;
        ChargerPresence getChargerPresence() const final;

        static BatteryWorkerQueue &getWorkerQueueHandle();

      private:
        void sendNotification(Events event);
        void pollFuelGauge();
        bool tryEnableCharging();

        xTimerHandle reinit_timer;
        xQueueHandle notification_channel;
        inline static std::unique_ptr<BatteryWorkerQueue> worker_queue;

        std::shared_ptr<drivers::DriverI2C> i2c;
        std::shared_ptr<drivers::DriverGPIO> charger_gpio_chgok;
        std::shared_ptr<drivers::DriverGPIO> charger_gpio;
        mutable bsp::devices::power::MP2639B charger;

        std::shared_ptr<drivers::DriverGPIO> fuel_gauge_gpio;
        mutable bsp::devices::power::CW2015 fuel_gauge;
    };

    BellBatteryCharger::BellBatteryCharger(xQueueHandle irqQueueHandle)
        : notification_channel{irqQueueHandle}, i2c{drivers::DriverI2C::Create(i2c_instance, i2c_params)},
          charger_gpio_chgok{drivers::DriverGPIO::Create(charger_irq_gpio_chgok, {})},
          charger_gpio{drivers::DriverGPIO::Create(charger_irq_gpio, {})}, charger{MP2639B::Configuration{
                                                                               charger_gpio,
                                                                               charger_irq_pin_mode,
                                                                               charger_gpio,
                                                                               charger_irq_pin_acok,
                                                                               charger_gpio_chgok,
                                                                               charger_irq_pin_chgok,
                                                                               [this](const auto) {
                                                                                   /// Due to this layer performing
                                                                                   /// charging control automatically,
                                                                                   /// there is no need to notify higher
                                                                                   /// layers about \ref
                                                                                   /// MP2639B::ChargingStatus::PluggedNotCharging
                                                                                   /// event.
                                                                                   if (not tryEnableCharging()) {
                                                                                       sendNotification(
                                                                                           Events::Charger);
                                                                                   }
                                                                               }}},

          fuel_gauge_gpio{drivers::DriverGPIO::Create(fuel_gauge_irq_gpio_instance, {})},
          fuel_gauge{CW2015{*i2c, battery_shutdown_threshold}}
    {

        reinit_timer = xTimerCreate("reinit_timer", reinit_poll_time, pdFALSE, this, [](TimerHandle_t xTimer) {
            BellBatteryCharger *inst = static_cast<BellBatteryCharger *>(pvTimerGetTimerID(xTimer));
            inst->fuel_gauge.reinit();
            inst->pollFuelGauge();
        });

        worker_queue = std::make_unique<BatteryWorkerQueue>("battery_charger", [this](const auto &msg) {
            switch (msg) {
            case IrqEvents::Charger: {
                charger.handle_irq();
            } break;

            case IrqEvents::FuelGauge: {
                fuel_gauge.irq_handler();
                sendNotification(Events::SOC);
            } break;
            }
        });

        tryEnableCharging();

        pollFuelGauge();

        charger.enable_irq();
        fuel_gauge.init_irq_pin(fuel_gauge_gpio, fuel_gauge_irq_pin);
    }

    AbstractBatteryCharger::Voltage BellBatteryCharger::getBatteryVoltage() const
    {
        constexpr std::uint8_t maxAttemps = 5;

        for (std::uint8_t i = 0; i < maxAttemps; ++i) {
            if (const auto result = fuel_gauge.get_battery_voltage()) {
                return *result;
            }
            i2c->ReInit();
            LOG_INFO("Attempting to get SoC data: %d", i);
            vTaskDelay(pdMS_TO_TICKS(i * 10));
        }
        LOG_ERROR("Error during fetching battery voltage");
        return 0;
    }

    std::optional<AbstractBatteryCharger::SOC> BellBatteryCharger::getSOC() const
    {
        constexpr std::uint8_t maxAttemps = 5;

        for (std::uint8_t i = 0; i < maxAttemps; ++i) {
            if (const auto result = fuel_gauge.get_battery_soc()) {
                const auto scaled_soc = scale_soc(*result);
                return *scaled_soc;
            }
            i2c->ReInit();
            LOG_INFO("Attempting to get SoC data: %d", i);
            vTaskDelay(pdMS_TO_TICKS(i * 10));
        }
        LOG_ERROR("Error during fetching battery SoC");
        return std::nullopt;
    }

    AbstractBatteryCharger::ChargingStatus BellBatteryCharger::getChargingStatus() const
    {
        /// Charger status fetched from MP2639B alone is not enough to be 100% sure what state we are currently in. For
        /// more info check the Table 2 (page 26) from the MP2639B datasheet. It is required to also take into account
        /// the current state of SOC.

        const auto charger_status = charger.get_charge_status();
        const auto current_soc    = getSOC();

        if (charger_status == MP2639B::ChargingStatus::Complete && current_soc >= 100) {
            return ChargingStatus::ChargingDone;
        }
        else if (charger_status == MP2639B::ChargingStatus::Discharging) {
            return ChargingStatus::Discharging;
        }
        else if (charger_status == MP2639B::ChargingStatus::Charging && current_soc < 100) {
            return ChargingStatus::Charging;
        }
        else if (charger_status == MP2639B::ChargingStatus::Complete && current_soc < 100) {
            return ChargingStatus::PluggedNotCharging;
        }
        else if (charger_status == MP2639B::ChargingStatus::Charging && current_soc >= 100) {
            LOG_INFO("The charger reports 'Charging state', but the battery SOC is 100");
            return ChargingStatus::ChargingDone;
        }
        else {
            return ChargingStatus::PluggedNotCharging;
        }
    }

    AbstractBatteryCharger::ChargerPresence BellBatteryCharger::getChargerPresence() const
    {
        return getChargingStatus() == ChargingStatus::Discharging ? AbstractBatteryCharger::ChargerPresence::Unplugged
                                                                  : AbstractBatteryCharger::ChargerPresence::PluggedIn;
    }

    BellBatteryCharger::BatteryWorkerQueue &BellBatteryCharger::getWorkerQueueHandle()
    {
        return *worker_queue;
    }
    void BellBatteryCharger::sendNotification(AbstractBatteryCharger::Events event)
    {
        xQueueSend(this->notification_channel, &event, default_timeout);
    }
    BellBatteryCharger::~BellBatteryCharger()
    {
        xTimerDelete(reinit_timer, default_timeout);
        fuel_gauge_gpio->DisableInterrupt(1 << fuel_gauge_irq_pin);
    }

    /// If the battery is completely discharged(SOC=0% and voltage=0mV) fuel gauge controller fails to perform the
    /// initialization procedure. In that case, give it 5 minutes to unlock the internal under-voltage
    /// protection. It will result in battery voltage jumping to around 2.5V which will be enough to initialize the
    /// chip.
    void BellBatteryCharger::pollFuelGauge()
    {
        if (not fuel_gauge) {
            LOG_WARN("Battery completely depleted, starting the re-initialization procedure");
            xTimerStart(reinit_timer, default_timeout);
        }
        else {
            LOG_INFO("Fuel gauge initialization success");
        }
    }

    bool BellBatteryCharger::tryEnableCharging()
    {
        if (charger.get_charge_status() == MP2639B::ChargingStatus::PluggedNotCharging) {
            charger.enable_charging(true);
            return true;
        }

        return false;
    }

    BaseType_t charger_irq()
    {
        return BellBatteryCharger::getWorkerQueueHandle().post(BellBatteryCharger::IrqEvents::Charger);
    }
    BaseType_t fuel_gauge_irq()
    {
        return BellBatteryCharger::getWorkerQueueHandle().post(BellBatteryCharger::IrqEvents::FuelGauge);
    }

    extern "C" void USB_ChargerDetectedCB(std::uint8_t /*unused*/)
    {
        /// It needs to be defined even if it is not used.
    }

    std::unique_ptr<AbstractBatteryCharger> AbstractBatteryCharger::Factory::create(xQueueHandle irqQueueHandle)
    {
        return std::make_unique<BellBatteryCharger>(irqQueueHandle);
    }

} // namespace hal::battery
