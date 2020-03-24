#include "ServiceLwIP.hpp"
#include <log/log.hpp>
#include "MessageType.hpp"
#include "Service/Message.hpp"
#include "Service/Service.hpp"

extern "C"
{
#include "dhcp-server/dhserver.h"
#include "lwip/apps/httpd.h"
#include "lwip/init.h"
#include "lwip/opt.h"
#include "lwip/tcpip.h"
};

#define NUM_DHCP_ENTRY 3

extern "C"
{
    static dhcp_entry_t entries[NUM_DHCP_ENTRY] = {
        /* mac    ip address        subnet mask        lease time */
        {{0}, {192, 168, 7, 2}, {255, 255, 255, 0}, 24 * 60 * 60},
        {{0}, {192, 168, 7, 3}, {255, 255, 255, 0}, 24 * 60 * 60},
        {{0}, {192, 168, 7, 4}, {255, 255, 255, 0}, 24 * 60 * 60}};

    static dhcp_config_t dhcp_config = {
        {192, 168, 7, 1},
        67,             /* server address, port */
        {0, 0, 0, 0},   /* dns server */
        NULL,           /* dns suffix */
        NUM_DHCP_ENTRY, /* num entry */
        entries         /* entries */
    };
};

sys::ReturnCodes message_lwip(sys::Service *app, LwIP_message::Request req)
{
    std::shared_ptr<LwIP_message> msg = std::make_shared<LwIP_message>(req);
    auto ret                          = sys::Bus::SendUnicast(msg, "ServiceLwIP", app, 5000);
    if (ret.first != sys::ReturnCodes::Success) {
        LOG_ERROR("err: %d", ret.first);
    }
    return ret.first;
}

const char *ServiceLwIP::serviceName = "ServiceLwIP";

ServiceLwIP::ServiceLwIP() : sys::Service(serviceName)
{
    LOG_INFO("[ServiceLwIP] Initializing");
    testTimerID = CreateTimer(3000, true);
    ReloadTimer(testTimerID);

    LOG_INFO("Start lwip!");
    tcpip_init(nullptr, nullptr);
    // lwip_init();
    dhserv_init(&dhcp_config);
    httpd_init();
}

void ServiceLwIP::TickHandler(uint32_t id)
{}

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
    LOG_ERROR("TRY START LWIP");
    try {
        switch (static_cast<MessageType>(msg->messageType)) {
        case MessageType::LwIP_request: {
            LwIP_message *lmsg = dynamic_cast<LwIP_message *>(msg);
            switch (lmsg->req) {
            case LwIP_message::Request::Start:
                LOG_ERROR("START LWIP");
                tcpip_init(nullptr, nullptr);
                // lwip_init();
                dhserv_init(&dhcp_config);
                httpd_init();
                LOG_INFO("LwIP started!");
                break;
            default:
                LOG_ERROR("Not implemented: %d", lmsg->req);
            }
        } break;
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
