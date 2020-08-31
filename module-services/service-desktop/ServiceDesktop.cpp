#include "ServiceDesktop.hpp"
#include "BackupRestore.hpp"
#include "DesktopMessages.hpp"
#include "EndpointHandler.hpp"
#include "FactoryReset.hpp"
#include "messages/DBSMSMessage.hpp"
#include "messages/DBSMSTemplateMessage.hpp"

ServiceDesktop::ServiceDesktop() : sys::Service(service::name::service_desktop, "", sdesktop::service_stack)
{
    LOG_INFO("[ServiceDesktop] Initializing");

    updateOS = std::make_unique<UpdatePureOS>(this);
}

ServiceDesktop::~ServiceDesktop()
{
    LOG_INFO("[ServiceDesktop] Cleaning resources");
    if (desktopWorker != nullptr) {
        desktopWorker->deinit();
    }
}

sys::ReturnCodes ServiceDesktop::InitHandler()
{
    desktopWorker = std::make_unique<WorkerDesktop>(this);
    desktopWorker->init(
        {{desktopWorker->RECEIVE_QUEUE_BUFFER_NAME, sizeof(std::string), sdesktop::cdc_queue_len},
         {desktopWorker->SEND_QUEUE_BUFFER_NAME, sizeof(std::string *), sdesktop::cdc_queue_object_size}});
    desktopWorker->run();

    connect(sdesktop::UpdateOsMessage(), [&](sys::DataMessage *msg, sys::ResponseMessage *resp) {
        sdesktop::UpdateOsMessage *updateOsMsg = dynamic_cast<sdesktop::UpdateOsMessage *>(msg);
        if (updateOsMsg != nullptr) {
            LOG_DEBUG("ServiceDesktop::DataReceivedHandler file:%s uuuid:%" PRIu32 "",
                      updateOsMsg->updateFile.c_str(),
                      updateOsMsg->uuid);

            if (updateOS->setUpdateFile(updateOsMsg->updateFile) == updateos::UpdateError::NoError)
                updateOS->runUpdate();
        }
        return std::make_shared<sys::ResponseMessage>();
    });

    connect(sdesktop::BackupMessage(), [&](sys::DataMessage *msg, sys::ResponseMessage *resp) {
        sdesktop::BackupMessage *backupMessage = dynamic_cast<sdesktop::BackupMessage *>(msg);
        if (backupMessage != nullptr) {
            LOG_DEBUG("ServiceDesktop::DataReceivedHandler BackupMessage received");
            BackupRestore::BackupUserFiles(this);
        }
        return std::make_shared<sys::ResponseMessage>();
    });

    connect(sdesktop::RestoreMessage(), [&](sys::DataMessage *msg, sys::ResponseMessage *resp) {
        sdesktop::RestoreMessage *restoreMessage = dynamic_cast<sdesktop::RestoreMessage *>(msg);
        if (restoreMessage != nullptr) {
            LOG_DEBUG("ServiceDesktop: RestoreMessage received");
            BackupRestore::RestoreUserFiles(this);
        }
        return std::make_shared<sys::ResponseMessage>();
    });

    connect(sdesktop::FactoryMessage(), [&](sys::DataMessage *msg, sys::ResponseMessage *resp) {
        auto *factoryMessage = dynamic_cast<sdesktop::FactoryMessage *>(msg);
        if (factoryMessage != nullptr) {
            LOG_DEBUG("ServiceDesktop: FactoryMessage received");
            FactoryReset::Run(this);
        }
        return std::make_shared<sys::ResponseMessage>();
    });

    vfs.updateTimestamp();

    return (sys::ReturnCodes::Success);
}

sys::ReturnCodes ServiceDesktop::DeinitHandler()
{
    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceDesktop::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    return sys::ReturnCodes::Success;
}

sys::Message_t ServiceDesktop::DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp)
{
    if (resp != nullptr) {
        if (resp->responseTo == MessageType::DBQuery) {
            EndpointHandler::handleQueryMessage(msg, resp);
        }
        else if (resp->responseTo >= MessageType::DBContactVerify && resp->responseTo <= MessageType::DBContactBlock) {
            EndpointHandler::handleContactsMessage(msg, resp);
        }
        else if (resp->responseTo >= MessageType::DBSMSAdd && resp->responseTo <= MessageType::DBSMSTemplateGetCount) {
            if (dynamic_cast<DBSMSResponseMessage *>(resp) != nullptr) {
                EndpointHandler::handleMessagesMessage(msg, resp);
            }
            else if (dynamic_cast<DBSMSTemplateResponseMessage *>(resp) != nullptr) {
                EndpointHandler::handleMessageTemplatesMessage(msg, resp);
            }
        }
        else {
            LOG_DEBUG("resp->ResponseTo: %d", static_cast<short>(resp->responseTo));
        }
    }

    return std::make_shared<sys::ResponseMessage>();
}
