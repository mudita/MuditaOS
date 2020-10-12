// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EventManagerServiceAPI.hpp"

#include <Service/Bus.hpp> // for Bus
#include <stdint.h>        // for uint32_t
#include <memory>          // for make_shared, shared_ptr, __shared_ptr<>::element_type
#include <utility>         // for pair

#include "../messages/EVMessages.hpp" // for EVMBoardResponseMessage
#include "../Constants.hpp"           // for evt_manager
#include "MessageType.hpp"            // for MessageType, MessageType::EVMGetBoard
#include "Service/Common.hpp"         // for ReturnCodes, ReturnCodes::Success
#include "Service/Message.hpp"        // for DataMessage

namespace sys
{
    class Service;
} // namespace sys

bsp::Board EventManagerServiceAPI::GetBoard(sys::Service *serv)
{
    constexpr uint32_t timeout = 1000;

    std::shared_ptr<sys::DataMessage> msg = std::make_shared<sys::DataMessage>(MessageType::EVMGetBoard);
    auto ret = sys::Bus::SendUnicast(msg, service::name::evt_manager, serv, pdMS_TO_TICKS(timeout));

    sevm::EVMBoardResponseMessage *response = dynamic_cast<sevm::EVMBoardResponseMessage *>(ret.second.get());

    if (response != nullptr) {
        if (ret.first == sys::ReturnCodes::Success) {
            return response->board;
        }
    }
    return bsp::Board::none;
}

void EventManagerServiceAPI::TurnVibration(sys::Service *serv, bsp::vibrator::State state)
{
    constexpr uint32_t timeout = 1000;

    sys::Bus::SendUnicast(
        std::make_shared<sevm::VibratorStateMessage>(state), service::name::evt_manager, serv, pdMS_TO_TICKS(timeout));
}

void EventManagerServiceAPI::PulseVibration(sys::Service *serv,
                                            std::chrono::milliseconds durationOn,
                                            std::chrono::milliseconds durationOff,
                                            bool forever)
{
    constexpr uint32_t timeout = 1000;

    sys::Bus::SendUnicast(std::make_shared<sevm::VibratorPulseMessage>(durationOn, durationOff, forever),
                          service::name::evt_manager,
                          serv,
                          pdMS_TO_TICKS(timeout));
}
