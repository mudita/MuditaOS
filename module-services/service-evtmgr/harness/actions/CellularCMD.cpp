#include "CellularCMD.hpp"
#include <service-cellular/ServiceCellular.hpp>
#include <service-cellular/api/CellularServiceAPI.hpp>

namespace harness
{
    namespace action
    {
        auto gsm_send(sys::Service *serv, const std::string &cmd) -> bool
        {
            auto msg = std::make_shared<cellular::RawCommand>();
            msg->command = cmd;
            msg->timeout = 3000; // TODO - from parameter or defult
            // TODO anywhere in system commands have 5s, for modem its seriously bad
            // as commands on modem can be much longer
            auto ret = sys::Bus::SendUnicast(msg, ServiceCellular::serviceName, serv, 5000);
            auto *response = dynamic_cast<cellular::RawCommandResp *>(ret.second.get());
            if (response == nullptr)
            {
                return true;
            }
            else
            {
                LOG_ERROR("Failed");
                return false;
            }
        }
    }; // namespace action
};     // namespace harness
