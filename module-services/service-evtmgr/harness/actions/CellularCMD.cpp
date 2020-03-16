#include "CellularCMD.hpp"
#include <service-cellular/ServiceCellular.hpp>
#include <service-cellular/api/CellularServiceAPI.hpp>

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
