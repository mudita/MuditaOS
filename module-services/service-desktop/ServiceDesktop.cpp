// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-db/QueryMessage.hpp>                            // for QueryResponse
#include <module-apps/application-desktop/ApplicationDesktop.hpp> // for name_desktop
#include <module-services/service-desktop/ServiceDesktop.hpp>
#include <cinttypes> // for PRIu32
#include <filesystem> // for path

#include "ServiceDesktop.hpp" // for ServiceDesktop, cdc_queue_len, cdc_queue_object_size, service_desktop, service_stack
#include "BackupRestore.hpp"  // for BackupRestore
#include "DesktopMessages.hpp" // for UpdateOsMessage, BackupMessage, FactoryMessage, RestoreMessage
#include "module-services/service-desktop/endpoints/factoryReset/FactoryReset.hpp" // for Run
#include "log/log.hpp"                                                             // for LOG_DEBUG, LOG_INFO, LOG_ERROR
#include "Common/Query.hpp"                                                        // for QueryResult
#include "MessageType.hpp"    // for MessageType, MessageType::DBQuery
#include "Service/Bus.hpp"    // for Bus
#include "Service/Worker.hpp" // for WorkerQueueInfo
#include "UpdateMuditaOS.hpp" // for UpdateMuditaOS, UpdateStats, UpdateMessageType, UpdateError, UpdateError::NoError, UpdateMessageType::UpdateCheckForUpdateOnce, UpdateMessageType::UpdateFoundOnBoot, UpdateMessageType::UpdateNow
#include "WorkerDesktop.hpp"  // for WorkerDesktop
#include "json/json11.hpp"    // for Json
#include "vfs.hpp"            // for vfs

ServiceDesktop::ServiceDesktop() : sys::Service(service::name::service_desktop, "", sdesktop::service_stack)
{
    LOG_INFO("[ServiceDesktop] Initializing");

    updateOS = std::make_unique<UpdateMuditaOS>(this);
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

    connect(sdesktop::developerMode::DeveloperModeRequest(), [&](sys::Message *msg) {
        auto request = static_cast<sdesktop::developerMode::DeveloperModeRequest *>(msg);
        if (request->event != nullptr) {
            request->event->send();
        }

        return std::make_shared<sys::ResponseMessage>();
    });

    connect(sdesktop::BackupMessage(), [&](sys::Message *msg) {
        sdesktop::BackupMessage *backupMessage = dynamic_cast<sdesktop::BackupMessage *>(msg);
        if (backupMessage != nullptr) {

            BackupRestore::BackupUserFiles(this);
        }
        return std::make_shared<sys::ResponseMessage>();
    });

    connect(sdesktop::RestoreMessage(), [&](sys::Message *msg) {
        sdesktop::RestoreMessage *restoreMessage = dynamic_cast<sdesktop::RestoreMessage *>(msg);
        if (restoreMessage != nullptr) {

            BackupRestore::RestoreUserFiles(this);
        }
        return std::make_shared<sys::ResponseMessage>();
    });

    connect(sdesktop::FactoryMessage(), [&](sys::Message *msg) {
        auto *factoryMessage = dynamic_cast<sdesktop::FactoryMessage *>(msg);
        if (factoryMessage != nullptr) {
            LOG_DEBUG("ServiceDesktop: FactoryMessage received");
            FactoryReset::Run(this);
        }
        return std::make_shared<sys::ResponseMessage>();
    });

    connect(sdesktop::UpdateOsMessage(), [&](sys::Message *msg) {
        sdesktop::UpdateOsMessage *updateOsMsg = dynamic_cast<sdesktop::UpdateOsMessage *>(msg);

        if (updateOsMsg != nullptr &&
            updateOsMsg->messageType == updateos::UpdateMessageType::UpdateCheckForUpdateOnce) {
            fs::path file = UpdateMuditaOS::checkForUpdate();

            if (file.has_filename()) {
                /* send info to applicationDesktop that there is an update waiting */
                auto msgToSend =
                    std::make_shared<sdesktop::UpdateOsMessage>(updateos::UpdateMessageType::UpdateFoundOnBoot, file);
                msgToSend->updateStats.versioInformation = UpdateMuditaOS::getVersionInfoFromFile(file);
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

    Store::BootConfig::modify().updateTimestamp();
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

sys::MessagePointer ServiceDesktop::DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp)
{
    if (auto msg = dynamic_cast<cellular::RawCommandResp *>(resp)) {
        auto event = std::make_unique<sdesktop::developerMode::ATResponseEvent>(msg->response);
        event->send();
    }
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
