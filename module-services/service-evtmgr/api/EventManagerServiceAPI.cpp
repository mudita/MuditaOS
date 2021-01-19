// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-evtmgr/EventManagerServiceAPI.hpp>
#include <service-evtmgr/EVMessages.hpp>
#include <service-evtmgr/Constants.hpp>

#include <MessageType.hpp>
#include <Service/Bus.hpp>
#include <Service/Common.hpp>
#include <Service/Message.hpp>

#include <cstdint>
#include <memory>
#include <utility>

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

void EventManagerServiceAPI::checkBatteryLevelCriticalState(sys::Service *serv)
{
    auto msg = std::make_shared<sevm::BatteryLevelCriticalCheckMessage>();
    sys::Bus::SendUnicast(msg, service::name::evt_manager, serv);
}
