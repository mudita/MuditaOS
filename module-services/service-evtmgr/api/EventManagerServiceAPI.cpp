// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EventManagerServiceAPI.hpp"

#include "MessageType.hpp" // for MessageType, MessageType::EVMG...

#include <Service/Bus.hpp> // for Bus
#include <stdint.h>        // for uint32_t

#include "../Constants.hpp"           // for evt_manager
#include "../messages/EVMessages.hpp" // for EVMBoardResponseMessage
#include "Service/Common.hpp"         // for ReturnCodes, ReturnCodes::Success
#include "Service/Message.hpp"        // for DataMessage
#include <memory>                     // for make_shared, shared_ptr, __sha...
#include <utility>                    // for pair

namespace sys
{
    class Service;
} // namespace sys

bsp::Board EventManagerServiceAPI::GetBoard(sys::Service *serv)
{
    constexpr uint32_t timeout = 1000;

    std::shared_ptr<sys::DataMessage> msg = std::make_shared<sys::DataMessage>(MessageType::EVMGetBoard);
    auto ret                              = sys::Bus::SendUnicast(msg, service::name::evt_manager, serv, timeout);

    sevm::EVMBoardResponseMessage *response = dynamic_cast<sevm::EVMBoardResponseMessage *>(ret.second.get());

    if (response != nullptr) {
        if (ret.first == sys::ReturnCodes::Success) {
            return response->board;
        }
    }
    return bsp::Board::none;
}
