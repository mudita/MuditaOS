// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AntennaServiceAPI.hpp"

#include "MessageType.hpp" // for MessageType, MessageTy...

#include "../messages/AntennaMessage.hpp"     // for AntennaLockRequestMessage
#include "Service/Bus.hpp"                    // for Bus
#include "Service/Common.hpp"                 // for ReturnCodes, ReturnCod...
#include "Service/Message.hpp"                // for DataMessage
#include "service-antenna/ServiceAntenna.hpp" // for ServiceAntenna, Servic...
#include <memory>                             // for make_shared, allocator
#include <utility>                            // for pair

namespace sys
{
    class Service;
} // namespace sys

namespace AntennaServiceAPI
{

    void CSQChange(sys::Service *serv)
    {
        auto msg = std::make_shared<sys::DataMessage>(MessageType::AntennaCSQChange);
        sys::Bus::SendMulticast(msg, sys::BusChannels::AntennaNotifications, serv);
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
