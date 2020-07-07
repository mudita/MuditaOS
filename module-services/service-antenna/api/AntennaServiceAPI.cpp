#include "AntennaServiceAPI.hpp"
#include "../messages/AntennaMessage.hpp"
namespace AntennaServiceAPI
{

    void CSQChange(sys::Service *serv)
    {
        auto msg = std::make_shared<sys::DataMessage>(MessageType::AntennaCSQChange);
        sys::Bus::SendUnicast(msg, ServiceAntenna::serviceName, serv);
    }
    bool LockRequest(sys::Service *serv, antenna::lockState request)
    {
        auto msg = std::make_shared<AntennaLockRequestMessage>(MessageType::AntennaLockService, request);
        auto ret = sys::Bus::SendUnicast(msg, ServiceAntenna::serviceName, serv, 5000);
        if (ret.first == sys::ReturnCodes::Success) {

            return true;
        }

        return false;
    }
    bool GetLockState(sys::Service *serv, antenna::lockState &response)
    {
        auto msg = std::make_shared<AntennaLockRequestMessage>(MessageType::AntennaGetLockState);
        auto ret = sys::Bus::SendUnicast(msg, ServiceAntenna::serviceName, serv, 5000);
        if (ret.first == sys::ReturnCodes::Success) {
            auto responseMsg = dynamic_cast<AntennaLockRequestResponse *>(ret.second.get());
            if (responseMsg != nullptr) {
                response = responseMsg->data;
                return true;
            }
        }
        return false;
    }
} // namespace AntennaServiceAPI
