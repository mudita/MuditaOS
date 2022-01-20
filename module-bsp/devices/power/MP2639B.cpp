// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MP2639B.hpp"

namespace bsp::devices::power
{
    MP2639B::MP2639B(const Configuration &conf) : configuration{conf}
    {
        irq_filter_timer =
            xTimerCreate("irq_charger_filter", pdMS_TO_TICKS(500), pdFALSE, this, [](TimerHandle_t xTimer) {
                MP2639B *inst = static_cast<MP2639B *>(pvTimerGetTimerID(xTimer));
                inst->configuration.notification(inst->get_charge_status());
            });

        drivers::DriverGPIOPinParams mode_pin_params{};
        mode_pin_params.dir      = drivers::DriverGPIOPinParams::Direction::Output;
        mode_pin_params.irqMode  = drivers::DriverGPIOPinParams::InterruptMode::NoIntmode;
        mode_pin_params.defLogic = 0;
        mode_pin_params.pin      = configuration.mode_pin;
        configuration.mode_gpio->ConfPin(mode_pin_params);

        /// Set the initial charging state
        enable_charging(configuration.acok_gpio->ReadPin(configuration.acok_pin) == 0u);
    }

    MP2639B::~MP2639B()
    {
        xTimerDelete(irq_filter_timer, pdMS_TO_TICKS(100));
        configuration.chgok_gpio->DisableInterrupt(1 << configuration.chgok_pin);
        configuration.acok_gpio->DisableInterrupt(1 << configuration.acok_pin);

        enable_charging(false);
    }
    MP2639B::ChargingStatus MP2639B::get_charge_status()
    {
        ChargingStatus status{};

        const auto acok  = configuration.acok_gpio->ReadPin(configuration.acok_pin);
        const auto chgok = configuration.chgok_gpio->ReadPin(configuration.chgok_pin);

        if (acok == 1 && chgok == 1) {
            status = ChargingStatus::Discharging;
        }
        else if (acok == 0 && chgok == 0) {
            status = ChargingStatus::Charging;
        }
        else if (acok == 0 && chgok == 1) {
            status = ChargingStatus::Complete;
        }
        // TODO: add error condition, i.e when chgok blinks at 1Hz
        return status;
    }
    void MP2639B::enable_charging(bool ctrl)
    {
        if (ctrl) {
            configuration.mode_gpio->WritePin(configuration.mode_pin, 0);
        }
        else {
            configuration.mode_gpio->WritePin(configuration.mode_pin, 1);
        }
    }
    void MP2639B::enable_irq()
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

        /* Enable interrupts */
        configuration.chgok_gpio->EnableInterrupt(1U << configuration.chgok_pin);
        configuration.acok_gpio->EnableInterrupt(1U << configuration.acok_pin);
    }

    void MP2639B::handle_irq()
    {
        enable_charging(configuration.acok_gpio->ReadPin(configuration.acok_pin) == 0u);

        if (xTimerIsTimerActive(irq_filter_timer) == pdFALSE) {
            xTimerStart(irq_filter_timer, pdMS_TO_TICKS(100));
        }
    }

} // namespace bsp::devices::power
