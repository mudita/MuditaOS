// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-db/DBNotificationMessage.hpp>
#include <service-db/DBServiceName.hpp>
#include <service-db/QueryMessage.hpp>
#include <service-db/ServiceDBCommon.hpp>

#include <purefs/filesystem_paths.hpp>

static const auto service_db_stack = 1024 * 24;

ServiceDBCommon::ServiceDBCommon() : sys::Service(service::name::db, "", service_db_stack, sys::ServicePriority::Idle)
{
    LOG_INFO("[ServiceDB] Initializing");
}

db::Interface *ServiceDBCommon::getInterface(db::Interface::Name interface)
{
    return nullptr;
}

// Invoked upon receiving data message
sys::MessagePointer ServiceDBCommon::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    std::shared_ptr<sys::ResponseMessage> responseMsg;
    auto type = static_cast<MessageType>(msgl->messageType);
    switch (type) {
    case MessageType::DBQuery: {
        auto msg = dynamic_cast<db::QueryMessage *>(msgl);
        assert(msg);
        db::Interface *interface = getInterface(msg->getInterface());
        assert(interface != nullptr);
        auto query     = msg->getQuery();
        auto queryType = query->type;
        auto result    = interface->runQuery(std::move(query));
        responseMsg    = std::make_shared<db::QueryResponse>(std::move(result));
        sendUpdateNotification(msg->getInterface(), queryType);
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
        return sys::ReturnCodes::Failure;
    }
    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceDBCommon::DeinitHandler()
{
    return sys::ReturnCodes::Success;
}

void ServiceDBCommon::ProcessCloseReason(sys::CloseReason closeReason)
{
    if (closeReason == sys::CloseReason::FactoryReset) {
        factoryReset();
    }
    sendCloseReadyMessage(this);
}

void ServiceDBCommon::factoryReset() const
{
    constexpr std::array fileExtensions = {".db", ".db-journal", ".db-wal"};

    LOG_INFO("Performing DB factory reset...");
    const auto userOSPath = purefs::dir::getUserDiskPath();
    for (const auto &f : std::filesystem::directory_iterator(userOSPath)) {
        if (const auto it = std::find(fileExtensions.begin(), fileExtensions.end(), f.path().extension());
            it != fileExtensions.end()) {
            if (const auto removeStatus = std::filesystem::remove(f.path()); !removeStatus) {
                LOG_ERROR("Unable to delete file: %s", f.path().c_str());
            }
            else {
                LOG_INFO("File deleted: %s", f.path().c_str());
            }
        }
    }
}

sys::ReturnCodes ServiceDBCommon::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    LOG_FATAL("[%s] PowerModeHandler: %s", this->GetName().c_str(), c_str(mode));
    return sys::ReturnCodes::Success;
}

void ServiceDBCommon::sendUpdateNotification(db::Interface::Name interface, db::Query::Type type)
{
    auto notificationMessage = std::make_shared<db::NotificationMessage>(interface, type);
    bus.sendMulticast(notificationMessage, sys::BusChannel::ServiceDBNotifications);
}
