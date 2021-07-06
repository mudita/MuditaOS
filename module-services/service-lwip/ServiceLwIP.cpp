// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-lwip/ServiceLwIP.hpp"

#include <MessageType.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <log.hpp>

extern "C"
{
#include "dhcp-server/dhserver.h" // for dhserv_init, dhcp_config_t, dhcp_entry_t
#include "lwip/apps/httpd.h"      // for httpd_init
#include "lwip/tcpip.h"           // for tcpip_init
};

#include <bits/exception.h>
#include <cstddef>
#include <memory>
#include <string>
#include <utility>

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
    auto ret                          = app->bus.sendUnicastSync(msg, "ServiceLwIP", 5000);
    if (ret.first != sys::ReturnCodes::Success) {
        LOG_ERROR("err: %s", c_str(ret.first));
    }
    return ret.first;
}

ServiceLwIP::ServiceLwIP() : sys::Service(service::name::lwip, "", StackDepth)
{
    LOG_INFO("[ServiceLwIP] Initializing");
    tcpip_init(nullptr, nullptr);
    // lwip_init();
    dhserv_init(&dhcp_config);
    httpd_init();
}

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

void ServiceLwIP::ProcessCloseReason(sys::CloseReason closeReason)
{
    sendCloseReadyMessage(this);
}

sys::MessagePointer ServiceLwIP::DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp)
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
            LOG_ERROR("Not handled messageType: %d", static_cast<int>(msg->messageType));
        }
    }
    catch (std::exception &ex) {
        LOG_ERROR("Exception on ServiceLwIP!: %s", ex.what());
    }
    return std::make_shared<sys::ResponseMessage>();
}

sys::ReturnCodes ServiceLwIP::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    LOG_ERROR("TODO");
    return sys::ReturnCodes::Success;
}
