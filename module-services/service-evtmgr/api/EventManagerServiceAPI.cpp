// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <service-evtmgr/EventManagerServiceAPI.hpp>
#include <service-evtmgr/EVMessages.hpp>
#include <service-evtmgr/ServiceEventManagerName.hpp>

#include <memory>

namespace sys
{
    class Service;
} // namespace sys

void EventManagerServiceAPI::vibrationPulseOnce(sys::Service *serv)
{
    serv->bus.sendUnicast(std::make_shared<sevm::VibratorMessage>(bsp::vibrator::Action::Pulse),
                          service::name::evt_manager);
}

void EventManagerServiceAPI::vibrationStop(sys::Service *serv)
{
    serv->bus.sendUnicast(std::make_shared<sevm::VibratorMessage>(bsp::vibrator::Action::Stop),
                          service::name::evt_manager);
}

void EventManagerServiceAPI::vibrationPulseRepeat(sys::Service *serv, std::chrono::milliseconds time)
{
    serv->bus.sendUnicast(std::make_shared<sevm::VibratorMessage>(bsp::vibrator::Action::PulseRepeat, time),
                          service::name::evt_manager);
}

void EventManagerServiceAPI::vibrationPulseRepeatUntilStop(sys::Service *serv)
{
    serv->bus.sendUnicast(std::make_shared<sevm::VibratorMessage>(bsp::vibrator::Action::PulseRepeatInfinite),
                          service::name::evt_manager);
}

void EventManagerServiceAPI::setVibrationLevel(sys::Service *serv, unsigned int vibrationLevel)
{
    serv->bus.sendUnicast(std::make_shared<sevm::VibratorLevelMessage>(vibrationLevel), service::name::evt_manager);
}

void EventManagerServiceAPI::turnOffTorch(sys::Service *serv)
{
    serv->bus.sendUnicast(std::make_shared<sevm::TurnOffTorchRequest>(), service::name::evt_manager);
}
