
#include "EventServiceAPI.hpp"
#include <Service/Service.hpp>
#include "../EventManager.hpp"
#include "../messages/EVMessages.hpp"

#include "../Constants.hpp"

bool EventServiceAPI::GetHwPlatform(sys::Service *serv)
{
    std::shared_ptr<sys::DataMessage> msg = std::make_shared<sys::DataMessage>(MessageType::EVMGetHw);
    auto ret                              = sys::Bus::SendUnicast(msg, service::name::evt_manager, serv, 1000);

    sevm::EVMResponseMessage *response = dynamic_cast<sevm::EVMResponseMessage *>(ret.second.get());

    if (response != nullptr) {
        if ((ret.first == sys::ReturnCodes::Success) && (response->retCode == true)) {
            return true;
        }
    }
    return false;
}
