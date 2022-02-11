// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryCharger.hpp"

#if ENABLE_CURRENT_MEASUREMENT_SCOPE
#include "CurrentMeasurementScope.hpp"
#endif

#include <bsp/battery_charger/battery_charger.hpp>
#include <hal/GenericFactory.hpp>

namespace
{
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
    BatteryCharger::BatteryCharger(AbstractBatteryCharger::BatteryChargerEvents &eventsHandler)
        : eventsHandler(eventsHandler),
          timerHandle{xTimerCreate(
              "clearIrqStatusTimer", pdMS_TO_TICKS(clearStatusTickTime_MS), false, nullptr, clearIrqStatusHandler)}
    {
#if ENABLE_CURRENT_MEASUREMENT_SCOPE
        CurrentMeasurementScope::start();
#endif
    }

    void BatteryCharger::init(xQueueHandle queueBatteryHandle, xQueueHandle queueChargerDetect)
    {
        bsp::battery_charger::init(queueBatteryHandle, queueChargerDetect);
    }

    void BatteryCharger::BatteryCharger::deinit()
    {
        bsp::battery_charger::deinit();
    }

    void BatteryCharger::processStateChangeNotification(std::uint8_t notification)
    {
        if (notification == static_cast<std::uint8_t>(bsp::battery_charger::batteryIRQSource::INTB) ||
            notification == static_cast<std::uint8_t>(bsp::battery_charger::batteryIRQSource::INOKB)) {
            checkBatteryChargerInterrupts();
        }
    }

    int BatteryCharger::getBatteryVoltage()
    {
        return bsp::battery_charger::getVoltageFilteredMeasurement();
    }

    void BatteryCharger::setChargingCurrentLimit(std::uint8_t chargerType)
    {
        using namespace bsp::battery_charger;
        switch (static_cast<batteryChargerType>(chargerType)) {
        case batteryChargerType::DcdTimeOut:
            [[fallthrough]];
        case batteryChargerType::DcdUnknownType:
            [[fallthrough]];
        case batteryChargerType::DcdError:
            [[fallthrough]];
        case batteryChargerType::DcdSDP:
            setMaxBusCurrent(USBCurrentLimit::lim500mA);
            break;
        case batteryChargerType::DcdCDP:
            [[fallthrough]];
        case batteryChargerType::DcdDCP:
            LOG_INFO("USB current limit set to 1000mA");
            setMaxBusCurrent(USBCurrentLimit::lim1000mA);
            break;
        }
    }

    void BatteryCharger::checkBatteryChargerInterrupts()
    {
        auto topINT = bsp::battery_charger::getTopControllerINTSource();
        if (topINT & static_cast<std::uint8_t>(bsp::battery_charger::topControllerIRQsource::CHGR_INT)) {
            bsp::battery_charger::getChargeStatus();
            bsp::battery_charger::actionIfChargerUnplugged();
            bsp::battery_charger::clearAllChargerIRQs();

            eventsHandler.onStatusChanged();
        }
        if (topINT & static_cast<std::uint8_t>(bsp::battery_charger::topControllerIRQsource::FG_INT)) {
            const auto status = bsp::battery_charger::getStatusRegister();
            if (status & static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::minVAlert)) {
                bsp::battery_charger::clearFuelGuageIRQ(
                    static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::minVAlert));

                if (bsp::battery_charger::getChargeStatus()) {
                    if (xTimerIsTimerActive(timerHandle) == pdFALSE) {
                        xTimerStart(timerHandle, 0);
                        LOG_DEBUG("Battery Brownout detected while charging");
                    }
                }
                else {
                    eventsHandler.onBrownout();
                }
            }
            if (status & static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::SOCOnePercentChange)) {
                bsp::battery_charger::printFuelGaugeInfo();
                bsp::battery_charger::clearFuelGuageIRQ(
                    static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::SOCOnePercentChange));
                bsp::battery_charger::getBatteryLevel();

                eventsHandler.onStatusChanged();
            }
            if (status & static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::maxTemp) ||
                status & static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::minTemp)) {
                bsp::battery_charger::clearFuelGuageIRQ(
                    static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::maxTemp) |
                    static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::minTemp));
                bsp::battery_charger::checkTemperatureRange();
                bsp::battery_charger::getChargeStatus();

                eventsHandler.onStatusChanged();
            }
            // Clear other unsupported IRQ sources just in case
            bsp::battery_charger::clearFuelGuageIRQ(
                static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::all));
        }
    }

    BaseType_t INTBHandlerIRQ()
    {
        return bsp::battery_charger::INTB_IRQHandler();
    }

    BaseType_t INOKBHandlerIRQ()
    {
        return bsp::battery_charger::INOKB_IRQHandler();
    }

    extern "C"
    {
        void USB_ChargerDetectedCB(std::uint8_t detectedType)
        {
            bsp::battery_charger::USBChargerDetectedHandler(detectedType);
        }
    }

} // namespace hal::battery
