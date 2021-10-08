// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "battery-charger.hpp"
#include "fsl_common.h"

#include <board/BoardDefinitions.hpp>
#include <EventStore.hpp>
#include <drivers/gpio/DriverGPIO.hpp>

namespace bsp::battery_charger
{
    namespace
    {
        std::shared_ptr<drivers::DriverGPIO> gpio;
        std::shared_ptr<drivers::DriverGPIO> gpio_int;

        static batteryRetval chargerStatus = batteryRetval::ChargerError;

        xQueueHandle IRQQueueHandle = nullptr;

    } // namespace

    int init(xQueueHandle irqQueueHandle)
    {
        IRQQueueHandle = irqQueueHandle;

        LOG_INFO("Initializing battery charger");

        gpio = drivers::DriverGPIO::Create(
            static_cast<drivers::GPIOInstances>(BoardDefinitions::BELL_BATTERY_CHARGER_GPIO),
            drivers::DriverGPIOParams{});

        drivers::DriverGPIOPinParams CHGENPinConfig;
        CHGENPinConfig.dir      = drivers::DriverGPIOPinParams::Direction::Output;
        CHGENPinConfig.irqMode  = drivers::DriverGPIOPinParams::InterruptMode::NoIntmode;
        CHGENPinConfig.defLogic = 0;
        CHGENPinConfig.pin      = static_cast<std::uint32_t>(BoardDefinitions::BELL_BATTERY_CHARGER_CHGEN_PIN);
        gpio->ConfPin(CHGENPinConfig);

        drivers::DriverGPIOPinParams ACOKPinConfig;
        ACOKPinConfig.dir      = drivers::DriverGPIOPinParams::Direction::Input;
        ACOKPinConfig.irqMode  = drivers::DriverGPIOPinParams::InterruptMode::IntFallingEdge;
        ACOKPinConfig.defLogic = 0;
        ACOKPinConfig.pin      = static_cast<std::uint32_t>(BoardDefinitions::BELL_BATTERY_CHARGER_ACOK_PIN);
        gpio->ConfPin(ACOKPinConfig);

        gpio_int = drivers::DriverGPIO::Create(
            static_cast<drivers::GPIOInstances>(BoardDefinitions::BELL_BATTERY_CHARGER_CHGOK_GPIO),
            drivers::DriverGPIOParams{});

        drivers::DriverGPIOPinParams CHGOKPinConfig;
        CHGOKPinConfig.dir      = drivers::DriverGPIOPinParams::Direction::Input;
        CHGOKPinConfig.irqMode  = drivers::DriverGPIOPinParams::InterruptMode::IntRisingOrFallingEdge;
        CHGOKPinConfig.defLogic = 0;
        CHGOKPinConfig.pin      = static_cast<std::uint32_t>(BoardDefinitions::BELL_BATTERY_CHARGER_CHGOK_PIN);
        gpio_int->ConfPin(CHGOKPinConfig);

        // set mode to discharge initially
        disableCharging();

        /* Enable GPIO pin interrupt */
        gpio_int->EnableInterrupt(1U << static_cast<uint32_t>(BoardDefinitions::BELL_BATTERY_CHARGER_CHGOK_PIN));
        gpio->EnableInterrupt(1U << static_cast<uint32_t>(BoardDefinitions::BELL_BATTERY_CHARGER_ACOK_PIN));

        enableCharging();
        getChargeStatus();

        return kStatus_Success;
    }

    void deinit()
    {
        gpio_int->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_BATTERY_CHARGER_CHGOK_PIN));
        gpio->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::BELL_BATTERY_CHARGER_ACOK_PIN));

        IRQQueueHandle = nullptr;

        disableCharging();

        gpio.reset();
        gpio_int.reset();
    }

    bool getChargeStatus()
    {
        uint8_t acok  = gpio->ReadPin(static_cast<uint32_t>(BoardDefinitions::BELL_BATTERY_CHARGER_ACOK_PIN));
        uint8_t chgok = gpio_int->ReadPin(static_cast<uint32_t>(BoardDefinitions::BELL_BATTERY_CHARGER_CHGOK_PIN));

        LOG_DEBUG("Charger status ACOK[%u], CHGOK[%u]", acok, chgok);

        if (acok != 0) {
            chargerStatus                  = batteryRetval::ChargerNotCharging;
            Store::Battery::modify().state = Store::Battery::State::Discharging;
            disableCharging();
            LOG_INFO("Discharging battery");
        }
        else if ((chgok == 0) && (acok == 0)) {
            chargerStatus                  = batteryRetval::ChargerCharging;
            Store::Battery::modify().state = Store::Battery::State::Charging;
            LOG_INFO("Charging battery");
        }
        else if ((chgok != 0) && (acok == 0)) {
            chargerStatus                  = batteryRetval::ChargerComplete;
            Store::Battery::modify().state = Store::Battery::State::ChargingDone;
            LOG_INFO("Battery charging complete");
        }

        return ((chargerStatus == batteryRetval::ChargerCharging) || (chargerStatus == batteryRetval::ChargerComplete));
    }

    void setChargerError()
    {
        chargerStatus = batteryRetval::ChargerError;
    }

    void enableCharging()
    {
        LOG_INFO("Enable charging");
        gpio->WritePin(static_cast<uint32_t>(BoardDefinitions::BELL_BATTERY_CHARGER_CHGEN_PIN), 0);
    }

    void disableCharging()
    {
        LOG_INFO("Disable charging");
        gpio->WritePin(static_cast<uint32_t>(BoardDefinitions::BELL_BATTERY_CHARGER_CHGEN_PIN), 1);
    }

    BaseType_t CHG_IRQHandler()
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        uint8_t val = 1;
        if (IRQQueueHandle) {
            xQueueSendFromISR(IRQQueueHandle, &val, &xHigherPriorityTaskWoken);
        }
        return xHigherPriorityTaskWoken;
    }
} // namespace bsp::battery_charger
