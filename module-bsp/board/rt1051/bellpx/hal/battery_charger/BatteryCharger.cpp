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

#include <variant>

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

    template <class... Ts>
    struct overloaded : Ts...
    {
        using Ts::operator()...;
    };
    template <class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;
} // namespace

namespace hal::battery
{
    class BellBatteryCharger : public AbstractBatteryCharger
    {
      public:
        struct events
        {
            enum class DCD : std::uint8_t
            {
                Timeout = 0x00U,
                UnknownType,
                Error, /// VBUS is coming from a dedicated power supply
                SDP,   /// Standard downstream port, up to 500mA
                CDP,   /// Charging downstream port, up to 1.5A
                DCP,   /// Dedicated charging port(wall warts and auto adapters without enumeration), up to 1.5A
            };
            struct Charger
            {};
            struct FuelGauge
            {};
        };
        using IrqEvents          = std::variant<events::DCD, events::Charger, events::FuelGauge>;
        using BatteryWorkerQueue = WorkerQueue<IrqEvents>;

        explicit BellBatteryCharger(xQueueHandle irqQueueHandle);
        ~BellBatteryCharger();

        std::optional<Voltage> getBatteryVoltage() const final;
        std::optional<SOC> getSOC() const final;
        ChargingStatus getChargingStatus() const final;
        ChargerPresence getChargerPresence() const final;
        TemperatureState getTemperatureState() const final;

        static BatteryWorkerQueue &getWorkerQueueHandle();

      private:
        template <typename Invokable, typename... Params>
        auto attemptToGetData(Invokable &&invokable, Params &&...params) const;

        void sendNotification(Events event);
        void pollFuelGauge();

        void handleIrqEvents(IrqEvents event);
        void handleChargerEvents(MP2639B::ChargingStatus status);

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
                                                                               [this](const auto event) {
                                                                                   handleChargerEvents(event);
                                                                               }}},

          fuel_gauge_gpio{drivers::DriverGPIO::Create(fuel_gauge_irq_gpio_instance, {})},
          fuel_gauge{CW2015{*i2c, battery_shutdown_threshold}}
    {

        reinit_timer = xTimerCreate("reinit_timer", reinit_poll_time, pdFALSE, this, [](TimerHandle_t xTimer) {
            BellBatteryCharger *inst = static_cast<BellBatteryCharger *>(pvTimerGetTimerID(xTimer));
            inst->fuel_gauge.reinit();
            inst->pollFuelGauge();
        });

        worker_queue = std::make_unique<BatteryWorkerQueue>(
            "battery_charger", [this](const auto &msg) { handleIrqEvents(msg); }, 1024);

        pollFuelGauge();

        charger.enable_irq();
        fuel_gauge.init_irq_pin(fuel_gauge_gpio, fuel_gauge_irq_pin);
    }

    template <typename Invokable, typename... Params>
    auto BellBatteryCharger::attemptToGetData(Invokable &&invokable, Params &&...params) const
    {
        constexpr std::uint8_t maxAttempts = 5;
        for (std::uint8_t i = 0; i < maxAttempts; ++i) {
            if (const auto result = std::invoke(invokable, std::forward<Params>(params)...)) {
                return result;
            }
            i2c->ReInit();
            LOG_INFO("Attempting to get I2C data: %d", i);
            vTaskDelay(pdMS_TO_TICKS(i * 10));
        }
        LOG_ERROR("Error during fetching I2C data");
        return std::result_of_t<Invokable(Params...)>{};
    }

    std::optional<AbstractBatteryCharger::Voltage> BellBatteryCharger::getBatteryVoltage() const
    {
        return attemptToGetData(&bsp::devices::power::CW2015::get_battery_voltage, fuel_gauge);
    }

    std::optional<AbstractBatteryCharger::SOC> BellBatteryCharger::getSOC() const
    {
        return attemptToGetData(&bsp::devices::power::CW2015::get_battery_soc, fuel_gauge);
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

    AbstractBatteryCharger::TemperatureState BellBatteryCharger::getTemperatureState() const
    {
        return AbstractBatteryCharger::TemperatureState::Normal; // Harmony has no battery temperature sensor
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
    void BellBatteryCharger::handleIrqEvents(const IrqEvents event)
    {
        std::visit(overloaded{[this](const events::Charger) { charger.handle_irq(); },
                              [this](const events::FuelGauge) {
                                  fuel_gauge.irq_handler();
                                  sendNotification(Events::SOC);
                              },
                              [this](const events::DCD evt) {
                                  LOG_INFO("USB charging port discovery result: %s",
                                           std::string{magic_enum::enum_name(evt)}.c_str());
                                  switch (evt) {
                                  case events::DCD::Timeout:
                                  case events::DCD::DCP:
                                      LOG_INFO("Valid charger detected, enabling charging");
                                      charger.enable_charging(true);
                                      break;
                                  case events::DCD::CDP:
                                  case events::DCD::SDP:
                                  default:
                                      charger.enable_charging(false);
                                  }
                              }},
                   event);
    }
    void BellBatteryCharger::handleChargerEvents(const MP2639B::ChargingStatus)
    {
        sendNotification(Events::Charger);
    }

    BaseType_t charger_irq()
    {
        return BellBatteryCharger::getWorkerQueueHandle().post({BellBatteryCharger::events::Charger{}});
    }
    BaseType_t fuel_gauge_irq()
    {
        return BellBatteryCharger::getWorkerQueueHandle().post({BellBatteryCharger::events::FuelGauge{}});
    }

    extern "C" void USB_ChargerDetectedCB(std::uint8_t raw_type)
    {
        BellBatteryCharger::getWorkerQueueHandle().post({BellBatteryCharger::events::DCD{raw_type}});
    }

    std::unique_ptr<AbstractBatteryCharger> AbstractBatteryCharger::Factory::create(xQueueHandle irqQueueHandle)
    {
        return std::make_unique<BellBatteryCharger>(irqQueueHandle);
    }
} // namespace hal::battery
