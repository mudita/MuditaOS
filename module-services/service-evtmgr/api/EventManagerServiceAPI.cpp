// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-evtmgr/EventManagerServiceAPI.hpp>
#include <service-evtmgr/EVMessages.hpp>
#include <service-evtmgr/Constants.hpp>

#include <MessageType.hpp>
#include <system/Common.hpp>
#include <Service/Message.hpp>

#include <cstdint>
#include <memory>

namespace sys
{
    class Service;
} // namespace sys

bsp::Board EventManagerServiceAPI::GetBoard(sys::Service *serv)
{
    constexpr uint32_t timeout = 1000;

    std::shared_ptr<sys::DataMessage> msg = std::make_shared<sys::DataMessage>(MessageType::EVMGetBoard);
    auto ret                              = serv->bus.sendUnicastSync(msg, service::name::evt_manager, timeout);

    sevm::EVMBoardResponseMessage *response = dynamic_cast<sevm::EVMBoardResponseMessage *>(ret.second.get());

    if (response != nullptr) {
        if (ret.first == sys::ReturnCodes::Success) {
            return response->board;
        }
    }
    return bsp::Board::none;
}

void EventManagerServiceAPI::vibrationPulseOnce(sys::Service *serv)
{
    serv->bus.sendUnicast(std::make_shared<sevm::VibratorMessage>(bsp::vibrator::Action::pulse),
                          service::name::evt_manager);
}

void EventManagerServiceAPI::vibrationStop(sys::Service *serv)
{
    serv->bus.sendUnicast(std::make_shared<sevm::VibratorMessage>(bsp::vibrator::Action::stop),
                          service::name::evt_manager);
}

void EventManagerServiceAPI::vibrationPulseRepeat(sys::Service *serv, std::chrono::milliseconds time)
{
    serv->bus.sendUnicast(std::make_shared<sevm::VibratorMessage>(bsp::vibrator::Action::pulseRepeat, time),
                          service::name::evt_manager);
}

void EventManagerServiceAPI::vibrationPulseRepeatUntilStop(sys::Service *serv)
{
    serv->bus.sendUnicast(std::make_shared<sevm::VibratorMessage>(bsp::vibrator::Action::pulseRepeatInfinite),
                          service::name::evt_manager);
}

void EventManagerServiceAPI::setVibrationLevel(sys::Service *serv, unsigned int vibrationLevel)
{
    serv->bus.sendUnicast(std::make_shared<sevm::VibratorLevelMessage>(vibrationLevel), service::name::evt_manager);
}
