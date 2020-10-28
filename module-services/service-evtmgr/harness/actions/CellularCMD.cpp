// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CellularCMD.hpp"

#include <Service/Bus.hpp>                      // for Bus
#include <service-cellular/ServiceCellular.hpp> // for ServiceCellular, ServiceCellular::serviceName
#include <memory>                               // for allocator, make_shared, __shared_ptr_access, shared_ptr

#include "log/log.hpp"                                   // for LOG_ERROR
#include "service-cellular/messages/CellularMessage.hpp" // for RawCommand

namespace sys
{
    class Service;
} // namespace sys

namespace harness
{
    namespace action
    {
        bool gsm_send(sys::Service *serv, const std::string &cmd)
        {
            auto msg     = std::make_shared<cellular::RawCommand>();
            msg->command = cmd;
            msg->timeout = 3000; // TODO - from parameter or defult
            auto ret     = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv);
            if (ret) {
                return true;
            }
            else {
                LOG_ERROR("Failed");
                return false;
            }
        }
    }; // namespace action
};     // namespace harness
