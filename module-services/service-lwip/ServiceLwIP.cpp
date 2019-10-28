#include "ServiceLwIP.hpp"
#include <log/log.hpp>
#include "MessageType.hpp"
#include "Service/Message.hpp"
#include "Service/Service.hpp"

extern "C" {
#include "dhserver.h"
#include "lwip/apps/httpd.h"
#include "lwip/init.h"
#include "lwip/opt.h"
#include "lwip/tcpip.h"
};

const char *ServiceLwIP::serviceName = "ServiceLwIP";

ServiceLwIP::ServiceLwIP() : sys::Service(serviceName)
{
    LOG_INFO("[ServiceLwIP] Initializing");
    testTimerID = CreateTimer(3000, true);
    ReloadTimer(testTimerID);
}

void ServiceLwIP::TickHandler(uint32_t id) {}

sys::ReturnCodes ServiceLwIP::InitHandler()
{
    LOG_ERROR("LwIP experimental!");
    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceLwIP::DeinitHandler()
{
    LOG_ERROR("TODO");
    return sys::ReturnCodes::Success;
}

sys::Message_t ServiceLwIP::DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp)
{
    try {
        switch (static_cast<MessageType>(msg->messageType)) {
        case MessageType::LwIP_request: {
            LwIP_message *lmsg = dynamic_cast<LwIP_message *>(msg);
            switch (msg->req) {
            case LwIP_message::Request::Start:
                lwip_init();
                dhserv_init(&dhcp_config);
                httpd_init();
                break;
            default:
                LOG_ERROR("Not implemented: %d", msg->req);
            }
        }
        default:
            LOG_ERROR("Not handled messageType: %s", msg->messageType);
        }
    }
    catch (std::exception &ex) {
        LOG_ERROR("Exception on BtService!: %s", ex.what());
    }
    return std::make_shared<sys::ResponseMessage>();
}

sys::ReturnCodes ServiceLwIP::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    LOG_ERROR("TODO");
    return sys::ReturnCodes::Success;
}
