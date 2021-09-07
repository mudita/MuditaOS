// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryCharger.hpp"

#include <hal/GenericFactory.hpp>

#include <bsp/battery_charger/battery_charger.hpp>
#include <service-evtmgr/battery-level-check/BatteryLevelCheck.hpp>
#include <service-evtmgr/BatteryMessages.hpp>
#include <SystemManager/SystemManagerCommon.hpp>
#include <service-evtmgr/EventManagerCommon.hpp>

namespace hal::battery
{
    std::shared_ptr<AbstractBatteryCharger> AbstractBatteryCharger::Factory::create(sys::Service *service)
    {
        return hal::impl::factory<BatteryCharger, AbstractBatteryCharger>(service);
    }

    BatteryCharger::BatteryCharger(sys::Service *service)
        : service{service}, batteryBrownoutDetector{
                                service,
                                []() { return bsp::battery_charger::getVoltageFilteredMeasurement(); },
                                [service]() {
                                    auto messageBrownout = std::make_shared<sevm::BatteryBrownoutMessage>();
                                    service->bus.sendUnicast(std::move(messageBrownout), service::name::system_manager);
                                },
                                [this]() { checkBatteryChargerInterrupts(); }}
    {}

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
        if (notification == static_cast<std::uint8_t>(bsp::battery_charger::batteryIRQSource::INTB)) {
            checkBatteryChargerInterrupts();
        }
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
            auto message = std::make_shared<sevm::BatteryStatusChangeMessage>();
            service->bus.sendUnicast(std::move(message), service::name::evt_manager);
            battery_level_check::checkBatteryLevel();
            bsp::battery_charger::clearAllChargerIRQs();
        }
        if (topINT & static_cast<std::uint8_t>(bsp::battery_charger::topControllerIRQsource::FG_INT)) {
            const auto status = bsp::battery_charger::getStatusRegister();
            if (status & static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::minVAlert)) {
                batteryBrownoutDetector.startDetection();
                bsp::battery_charger::clearFuelGuageIRQ(
                    static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::minVAlert));
            }
            if (status & static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::SOCOnePercentChange)) {
                bsp::battery_charger::printFuelGaugeInfo();
                bsp::battery_charger::clearFuelGuageIRQ(
                    static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::SOCOnePercentChange));
                bsp::battery_charger::getBatteryLevel();
                auto message = std::make_shared<sevm::BatteryStatusChangeMessage>();
                service->bus.sendUnicast(std::move(message), service::name::evt_manager);
                battery_level_check::checkBatteryLevel();
            }
            if (status & static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::maxTemp) ||
                status & static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::minTemp)) {
                bsp::battery_charger::clearFuelGuageIRQ(
                    static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::maxTemp) |
                    static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::minTemp));
                bsp::battery_charger::checkTemperatureRange();
                bsp::battery_charger::getChargeStatus();
                auto message = std::make_shared<sevm::BatteryStatusChangeMessage>();
                service->bus.sendUnicast(std::move(message), service::name::evt_manager);
                battery_level_check::checkBatteryLevel();
            }
            // Clear other unsupported IRQ sources just in case
            bsp::battery_charger::clearFuelGuageIRQ(
                static_cast<std::uint16_t>(bsp::battery_charger::batteryINTBSource::all));
        }
    }

    BaseType_t IRQHandler()
    {
        return bsp::battery_charger::INTB_IRQHandler();
    }

    extern "C"
    {
        void USB_ChargerDetectedCB(std::uint8_t detectedType)
        {
            bsp::battery_charger::USBChargerDetectedHandler(detectedType);
        }
    }

} // namespace hal::battery
