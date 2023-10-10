// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-db/DBNotificationMessage.hpp>
#include <service-db/DBServiceName.hpp>
#include <service-db/QueryMessage.hpp>
#include <service-db/ServiceDBCommon.hpp>

#include <purefs/filesystem_paths.hpp>
#include <log/log.hpp>

namespace
{
    constexpr auto serviceDbStackSize = 1024 * 24;
}

ServiceDBCommon::ServiceDBCommon() : sys::Service(service::name::db, "", serviceDbStackSize, sys::ServicePriority::Idle)
{
}

db::Interface *ServiceDBCommon::getInterface(db::Interface::Name interface)
{
    return nullptr;
}

// Invoked upon receiving data message
sys::MessagePointer ServiceDBCommon::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    std::shared_ptr<sys::ResponseMessage> responseMsg;
    const auto type = static_cast<MessageType>(msgl->messageType);
    switch (type) {
    case MessageType::DBQuery: {
        const auto msg = dynamic_cast<db::QueryMessage *>(msgl);
        assert(msg);

        const auto interface = getInterface(msg->getInterface());
        assert(interface != nullptr);

        const std::shared_ptr<db::Query> query(std::move(msg->getQuery()));
        auto result = interface->runQuery(query);
        std::optional<std::uint32_t> id;
        if (result != nullptr) {
            id = result->getRecordID();
        }
        else {
            LOG_WARN("There is no response associated with query: %s!", query ? query->debugInfo().c_str() : "");
        }
        responseMsg = std::make_shared<db::QueryResponse>(std::move(result));
        sendUpdateNotification(msg->getInterface(), query->type, id);
    } break;

    default:
        break;
    }

    if (responseMsg == nullptr) {
        return responseMsg;
    }

    responseMsg->responseTo = msgl->messageType;
    return responseMsg;
}

sys::ReturnCodes ServiceDBCommon::InitHandler()
{
    if (const auto isSuccess = Database::initialize(); !isSuccess) {
        LOG_ERROR("Failed to initialize");

        return sys::ReturnCodes::Failure;
    }
    LOG_INFO("Initialized");
    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceDBCommon::DeinitHandler()
{
    for (auto &dbAgent : databaseAgents) {
        dbAgent->unRegisterMessages();
    }

    if (const auto isSuccess = Database::deinitialize(); !isSuccess) {
        return sys::ReturnCodes::Failure;
    }
    LOG_INFO("Deinitialized");

    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceDBCommon::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    LOG_FATAL("[%s] PowerModeHandler: %s", this->GetName().c_str(), c_str(mode));
    return sys::ReturnCodes::Success;
}

void ServiceDBCommon::sendUpdateNotification(db::Interface::Name interface,
                                             db::Query::Type type,
                                             std::optional<std::uint32_t> recordId)
{
    auto notificationMessage = std::make_shared<db::NotificationMessage>(interface, type, recordId);
    bus.sendMulticast(notificationMessage, sys::BusChannel::ServiceDBNotifications);
}
