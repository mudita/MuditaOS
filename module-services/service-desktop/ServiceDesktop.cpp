// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <messages/QueryMessage.hpp>
#include <module-apps/application-desktop/ApplicationDesktop.hpp>
#include "ServiceDesktop.hpp"
#include "BackupRestore.hpp"
#include "DesktopMessages.hpp"
#include "module-services/service-desktop/endpoints/factoryReset/FactoryReset.hpp"
#include "log/log.hpp"

ServiceDesktop::ServiceDesktop() : sys::Service(service::name::service_desktop, "", sdesktop::service_stack)
{
    LOG_INFO("[ServiceDesktop] Initializing");

    updateOS = std::make_unique<UpdatePureOS>(this);
}

ServiceDesktop::~ServiceDesktop()
{
    LOG_INFO("[ServiceDesktop] Cleaning resources");
}

sys::ReturnCodes ServiceDesktop::InitHandler()
{
    desktopWorker = std::make_unique<WorkerDesktop>(this);
    desktopWorker->init(
        {{desktopWorker->RECEIVE_QUEUE_BUFFER_NAME, sizeof(std::string), sdesktop::cdc_queue_len},
         {desktopWorker->SEND_QUEUE_BUFFER_NAME, sizeof(std::string *), sdesktop::cdc_queue_object_size}});
    desktopWorker->run();

    connect(sdesktop::BackupMessage(), [&](sys::DataMessage *msg, sys::ResponseMessage *resp) {
        sdesktop::BackupMessage *backupMessage = dynamic_cast<sdesktop::BackupMessage *>(msg);
        if (backupMessage != nullptr) {

            BackupRestore::BackupUserFiles(this);
        }
        return std::make_shared<sys::ResponseMessage>();
    });

    connect(sdesktop::RestoreMessage(), [&](sys::DataMessage *msg, sys::ResponseMessage *resp) {
        sdesktop::RestoreMessage *restoreMessage = dynamic_cast<sdesktop::RestoreMessage *>(msg);
        if (restoreMessage != nullptr) {

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

    connect(sdesktop::UpdateOsMessage(), [&](sys::DataMessage *msg, sys::ResponseMessage *resp) {
        sdesktop::UpdateOsMessage *updateOsMsg = dynamic_cast<sdesktop::UpdateOsMessage *>(msg);

        if (updateOsMsg != nullptr &&
            updateOsMsg->messageType == updateos::UpdateMessageType::UpdateCheckForUpdateOnce) {
            fs::path file = UpdatePureOS::checkForUpdate();

            if (file.has_filename()) {
                /* send info to applicationDesktop that there is an update waiting */
                auto msgToSend =
                    std::make_shared<sdesktop::UpdateOsMessage>(updateos::UpdateMessageType::UpdateFoundOnBoot, file);
                msgToSend->updateStats.versioInformation = UpdatePureOS::getVersionInfoFromFile(file);
                sys::Bus::SendUnicast(msgToSend, app::name_desktop, this);
            }
        }

        if (updateOsMsg != nullptr && updateOsMsg->messageType == updateos::UpdateMessageType::UpdateNow) {
            LOG_DEBUG("ServiceDesktop::DataReceivedHandler file:%s uuuid:%" PRIu32 "",
                      updateOsMsg->updateStats.updateFile.c_str(),
                      updateOsMsg->updateStats.uuid);

            if (updateOS->setUpdateFile(updateOsMsg->updateStats.updateFile) == updateos::UpdateError::NoError)
                updateOS->runUpdate();
        }
        return std::make_shared<sys::ResponseMessage>();
    });

    vfs.updateTimestamp();

    return (sys::ReturnCodes::Success);
}

sys::ReturnCodes ServiceDesktop::DeinitHandler()
{
    desktopWorker->close();
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
            if (auto queryResponse = dynamic_cast<db::QueryResponse *>(resp)) {
                auto result = queryResponse->getResult();
                if (result != nullptr) {
                    LOG_DEBUG("Result: %s", result->debugInfo().c_str());
                    if (result->hasListener()) {
                        LOG_DEBUG("Handling result...");
                        result->handle();
                    }
                }
                else {
                    LOG_ERROR("Wrong result - nullptr!");
                }
            }
        }
    }

    return std::make_shared<sys::ResponseMessage>();
}
