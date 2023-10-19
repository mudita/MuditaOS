// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BatteryController.hpp"
#include "system/Constants.hpp"

#if ENABLE_CURRENT_MEASUREMENT_SCOPE
#include "CurrentMeasurementScope.hpp"
#endif

#include <service-evtmgr/BatteryMessages.hpp>
#include <service-evtmgr/ServiceEventManagerName.hpp>
#include <service-appmgr/ServiceApplicationManagerName.hpp>
#include <service-desktop/ServiceDesktopName.hpp>
#include <module-utils/EventStore/EventStore.hpp>
#include <log/log.hpp>
#include <magic_enum.hpp>

using sevm::battery::BatteryController;

namespace
{
    using hal::battery::AbstractBatteryCharger;
    Store::Battery::State transformChargingState(AbstractBatteryCharger::ChargingStatus status)
    {
        using Status   = AbstractBatteryCharger::ChargingStatus;
        using NewState = Store::Battery::State;
        switch (status) {
        case Status::Discharging:
            return NewState::Discharging;
        case Status::Charging:
            return NewState::Charging;
        case Status::ChargingDone:
            return NewState::ChargingDone;
        case Status::PluggedNotCharging:
            return NewState::PluggedNotCharging;
        default:
            return NewState::Discharging;
        }
    }

    BatteryState::ChargingState transformChargingState(Store::Battery::State status)
    {
        using Status   = Store::Battery::State;
        using NewState = BatteryState::ChargingState;
        switch (status) {
        case Status::Discharging:
            return NewState::Discharging;
        case Status::Charging:
            return NewState::Charging;
        case Status::ChargingDone:
            return NewState::ChargingDone;
        case Status::PluggedNotCharging:
            return NewState::PluggedNotCharging;
        default:
            return NewState::Discharging;
        }
    }

    Store::Battery::LevelState transformBatteryState(BatteryState::State status)
    {
        using Status   = BatteryState::State;
        using NewState = Store::Battery::LevelState;
        switch (status) {
        case Status::Normal:
            return NewState::Normal;
        case Status::Shutdown:
            return NewState::Shutdown;
        case Status::CriticalCharging:
            return NewState::CriticalCharging;
        case Status::CriticalNotCharging:
            return NewState::CriticalNotCharging;
        default:
            return NewState::Normal;
        }
    }

    Store::Battery::Temperature transformTemperatureState(AbstractBatteryCharger::TemperatureState state)
    {
        using State    = AbstractBatteryCharger::TemperatureState;
        using NewState = Store::Battery::Temperature;
        switch (state) {
        case State::Normal:
            return NewState::Normal;
        case State::TooLow:
            return NewState::TooLow;
        case State::TooHigh:
            return NewState::TooHigh;
        default:
            return NewState::Unknown;
        }
    }

    bool isProperLastState(Store::Battery::State state)
    {
        return (state == Store::Battery::State::Discharging) || (state == Store::Battery::State::PluggedNotCharging);
    }

    bool isProperCurrentState(Store::Battery::State state)
    {
        return (state == Store::Battery::State::Charging) || (state == Store::Battery::State::ChargingDone);
    }
} // namespace

BatteryController::BatteryController(sys::Service *service, xQueueHandle notificationChannel, EventManagerParams params)
    : service{service}, charger{hal::battery::AbstractBatteryCharger::Factory::create(notificationChannel)},
      brownoutDetector(
          service,
          *charger,
          params.voltage,
          [this](BatteryBrownoutDetector::DetectionResult result) {
              using Brownout = BatteryBrownoutDetector::DetectionResult;
              switch (result) {
              case Brownout::eventDetected: {
                  auto messageBrownout = std::make_shared<sevm::BatteryBrownoutMessage>();
                  this->service->bus.sendUnicast(std::move(messageBrownout), service::name::system_manager);
                  LOG_INFO("Battery brownout %s. Sending message...", magic_enum::enum_name(result).data());
              } break;
              case Brownout::detectionNegative:
              default:
                  LOG_INFO("Battery brownout detection window finish with negative result");
                  break;
              }
          }),
      batteryState{service,
                   [this](const auto state) {
                       Store::Battery::modify().levelState = transformBatteryState(state);
                       auto stateChangeMessage             = std::make_shared<sevm::BatteryStateChangeMessage>(state);
                       this->service->bus.sendMulticast(std::move(stateChangeMessage),
                                                        sys::BusChannel::ServiceEvtmgrNotifications);
                   },
                   params.battery}
{
    updateSoc();
    Store::Battery::modify().state = transformChargingState(charger->getChargingStatus());
    Store::Battery::modify().temperature = transformTemperatureState(charger->getTemperatureState());
    batteryState.check(transformChargingState(Store::Battery::get().state),
                       static_cast<float>(Store::Battery::get().level));

    LOG_INFO("Initial charger state: %s", magic_enum::enum_name(Store::Battery::get().state).data());
    LOG_INFO("Initial battery SOC: %d", Store::Battery::get().level);
    LOG_INFO("Initial battery voltage: %" PRIu32 "mV", getVoltage());
    LOG_INFO("Initial battery state: %s", magic_enum::enum_name(Store::Battery::get().levelState).data());
    LOG_INFO("Initial battery temperature range: %s", magic_enum::enum_name(Store::Battery::get().temperature).data());
}

void sevm::battery::BatteryController::handleNotification(Events evt)
{
    LOG_INFO("Incoming event: %s", magic_enum::enum_name(evt).data());
    switch (evt) {
    case Events::Charger:
        checkChargerPresence();
        update();
        break;
    case Events::SOC:
        update();
        break;
    case Events::Brownout:
        brownoutDetector.poll();
        break;
    }
}

void sevm::battery::BatteryController::poll()
{
    update();
    brownoutDetector.poll();
}

void sevm::battery::BatteryController::printCurrentState()
{
    LOG_INFO("Charger state: %s | Battery SOC: %d%% | Voltage: %" PRIu32 "mV | Level state: %s | Temperature: %s",
             magic_enum::enum_name(Store::Battery::get().state).data(),
             Store::Battery::get().level,
             getVoltage(),
             magic_enum::enum_name(Store::Battery::get().levelState).data(),
             magic_enum::enum_name(Store::Battery::get().temperature).data());
}

void sevm::battery::BatteryController::update()
{
    const auto lastSoc   = Store::Battery::get().level;
    const auto lastState = Store::Battery::get().state;
    const auto lastTemperature = Store::Battery::get().temperature;

    updateSoc();
    Store::Battery::modify().state = transformChargingState(charger->getChargingStatus());
    Store::Battery::modify().temperature = transformTemperatureState(charger->getTemperatureState());

    const auto currentSoc         = Store::Battery::get().level;
    const auto currentState       = Store::Battery::get().state;
    const auto currentTemperature = Store::Battery::get().temperature;

    /// Send BatteryStatusChangeMessage only when battery SOC, charger state or temperature has changed
    if ((lastSoc != currentSoc) || (lastState != currentState) || (lastTemperature != currentTemperature)) {
        auto message = std::make_shared<sevm::BatteryStatusChangeMessage>();
        service->bus.sendUnicast(std::move(message), service::name::evt_manager);
    }

    if (isFirstUpdateDone) {
        sendChargingNotification(lastState, currentState);
    }
    else {
        if (lastState == Store::Battery::State::Discharging) {
            sendChargingNotification(lastState, currentState);
        }
        isFirstUpdateDone = true;
    }

    batteryState.check(transformChargingState(Store::Battery::get().state),
                       static_cast<float>(Store::Battery::get().level));

    printCurrentState();
}

void sevm::battery::BatteryController::updateSoc()
{
    const auto batteryLevel = charger->getSOC();
    if (batteryLevel) {
        Store::Battery::modify().level = batteryLevel.value();
    }
}

units::Voltage sevm::battery::BatteryController::getVoltage()
{
    const auto voltage = charger->getBatteryVoltage();
    if (voltage) {
        return *voltage;
    }
    LOG_ERROR("Can't get the voltage");
    return 0;
}

void sevm::battery::BatteryController::checkChargerPresence()
{
    using PlugEvent = sevm::USBPlugEvent::Event;

    const auto newChargerPresence = charger->getChargerPresence();
    if (chargerPresence == newChargerPresence) {
        return;
    }

    const auto plugEvent =
        (newChargerPresence == ChargerPresence::PluggedIn) ? PlugEvent::CablePlugged : PlugEvent::CableUnplugged;
    service->bus.sendUnicast(std::make_shared<sevm::USBPlugEvent>(plugEvent), service::name::service_desktop);
    chargerPresence = newChargerPresence;
}

void sevm::battery::BatteryController::sendChargingNotification(const Store::Battery::State &lastState,
                                                                const Store::Battery::State &currentState)
{
    if (isProperLastState(lastState) && isProperCurrentState(currentState)) {
        service->bus.sendUnicast(std::make_shared<sevm::BatteryChargingMessage>(transformChargingState(currentState)),
                                 service::name::appmgr);
    }
}
