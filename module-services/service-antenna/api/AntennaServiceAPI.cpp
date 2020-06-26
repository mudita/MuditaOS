#include "AntennaServiceAPI.hpp"

namespace AntennaServiceAPI
{

    void CSQChange(sys::Service *serv)
    {
        std::shared_ptr<sys::DataMessage> msg = std::make_shared<sys::DataMessage>(MessageType::AntennaCSQChange);
        sys::Bus::SendUnicast(msg, ServiceAntenna::serviceName, serv);
    }
} // namespace AntennaServiceAPI
