// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MP2615GQ.hpp"

namespace bsp::devices::power
{
    MP2615GQ::MP2615GQ(const Configuration &conf) : configuration{conf}
    {
        irq_filter_timer = xTimerCreate(irq_filter_timer_name,
                                        pdMS_TO_TICKS(irq_filter_timer_interval_ms),
                                        pdFALSE,
                                        this,
                                        [](TimerHandle_t xTimer) {
                                            auto inst = static_cast<MP2615GQ *>(pvTimerGetTimerID(xTimer));
                                            /// Charging disable is done automatically if the conditions are met.
                                            if (!inst->is_valid_voltage()) {
                                                inst->enable_charging(false);
                                            }
                                            if (inst->configuration.notification) {
                                                inst->configuration.notification(inst->get_charge_status());
                                            }
                                        });

        drivers::DriverGPIOPinParams mode_pin_params{};
        mode_pin_params.dir      = drivers::DriverGPIOPinParams::Direction::Output;
        mode_pin_params.irqMode  = drivers::DriverGPIOPinParams::InterruptMode::NoIntmode;
        mode_pin_params.defLogic = 0;
        mode_pin_params.pin      = configuration.mode_pin;
        configuration.mode_gpio->ConfPin(mode_pin_params);

        /// Always start with charging disabled
        enable_charging(false);
    }

    MP2615GQ::~MP2615GQ()
    {
        xTimerDelete(irq_filter_timer, pdMS_TO_TICKS(irq_filter_timer_cmd_timeout_ms));
        configuration.chgok_gpio->DisableInterrupt(1 << configuration.chgok_pin);
        configuration.acok_gpio->DisableInterrupt(1 << configuration.acok_pin);

        enable_charging(false);
    }

    MP2615GQ::ChargingStatus MP2615GQ::get_charge_status()
    {
        /// For more info, please refer to the Table 1 "Charging Status Indication" in reference manual.
        const auto valid_voltage = is_valid_voltage();
        const auto charging      = is_charging();

        ChargingStatus status{};
        if (!valid_voltage) {
            status = ChargingStatus::Discharging;
        }
        else if (is_charging_enabled()) {
            status = charging ? ChargingStatus::Charging : ChargingStatus::Complete;
        }
        else if (!is_charging_enabled()) {
            status = ChargingStatus::PluggedNotCharging;
        }
        return status;
    }

    void MP2615GQ::enable_charging(bool ctrl)
    {
        configuration.mode_gpio->WritePin(configuration.mode_pin, ctrl ? 0 : 1);
    }

    void MP2615GQ::enable_irq()
    {
        drivers::DriverGPIOPinParams ACOKPinConfig{};
        ACOKPinConfig.dir      = drivers::DriverGPIOPinParams::Direction::Input;
        ACOKPinConfig.irqMode  = drivers::DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge;
        ACOKPinConfig.defLogic = 0;
        ACOKPinConfig.pin      = configuration.acok_pin;
        configuration.acok_gpio->ConfPin(ACOKPinConfig);

        drivers::DriverGPIOPinParams CHGOKPinConfig{};
        CHGOKPinConfig.dir      = drivers::DriverGPIOPinParams::Direction::Input;
        CHGOKPinConfig.irqMode  = drivers::DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge;
        CHGOKPinConfig.defLogic = 0;
        CHGOKPinConfig.pin      = configuration.chgok_pin;
        configuration.chgok_gpio->ConfPin(CHGOKPinConfig);

        configuration.chgok_gpio->EnableInterrupt(1U << configuration.chgok_pin);
        configuration.acok_gpio->EnableInterrupt(1U << configuration.acok_pin);
    }

    void MP2615GQ::handle_irq()
    {
        if (xTimerIsTimerActive(irq_filter_timer) == pdFALSE) {
            xTimerStart(irq_filter_timer, pdMS_TO_TICKS(irq_filter_timer_cmd_timeout_ms));
        }
    }

    bool MP2615GQ::is_charging_enabled() const
    {
        return configuration.mode_gpio->ReadPin(configuration.mode_pin) == 0;
    }

    bool MP2615GQ::is_valid_voltage() const
    {
        /// ACOK -> Valid Input Supply Indicator. A logic Low on this pin indicates the presence of a valid input power
        /// supply.
        return configuration.acok_gpio->ReadPin(configuration.acok_pin) == 0;
    }

    bool MP2615GQ::is_charging() const
    {
        /// CHGOK -> Charging Completion Indicator. A logic Low indicates charging operation. The pin will become an
        /// open drain once the charge is completed or suspended.
        return configuration.chgok_gpio->ReadPin(configuration.chgok_pin) == 0;
    }
} // namespace bsp::devices::power
