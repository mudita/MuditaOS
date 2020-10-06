
#include "EventManagerServiceAPI.hpp"
#include <Service/Service.hpp>
#include "../EventManager.hpp"
#include "../messages/EVMessages.hpp"

#include "../Constants.hpp"
#include <Service/Bus.hpp>

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
