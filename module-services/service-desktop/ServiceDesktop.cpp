﻿// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-desktop/DesktopMessages.hpp"
#include "service-desktop/ServiceDesktop.hpp"
#include "service-desktop/WorkerDesktop.hpp"
#include "service-cellular/CellularMessage.hpp"
#include "endpoints/factoryReset/FactoryReset.hpp"
#include "endpoints/backup/BackupRestore.hpp"
#include "endpoints/update/UpdateMuditaOS.hpp"

#include <Common/Query.hpp>
#include <MessageType.hpp>
#include <Service/Worker.hpp>
#include <json/json11.hpp>
#include <log/log.hpp>
#include <module-apps/application-desktop/ApplicationDesktop.hpp>
#include <service-db/service-db/Settings.hpp>
#include <service-db/QueryMessage.hpp>

#include <cinttypes>
#include <filesystem>

ServiceDesktop::ServiceDesktop() : sys::Service(service::name::service_desktop, "", sdesktop::service_stack)
{
    LOG_INFO("[ServiceDesktop] Initializing");

    updateOS = std::make_unique<UpdateMuditaOS>(this);
    settings = std::make_unique<settings::Settings>(this);

}

ServiceDesktop::~ServiceDesktop()
{
    LOG_INFO("[ServiceDesktop] Cleaning resources");
}

sys::ReturnCodes ServiceDesktop::InitHandler()
{
    desktopWorker = std::make_unique<WorkerDesktop>(this);
    const bool ret = desktopWorker->init(
        {{sdesktop::RECEIVE_QUEUE_BUFFER_NAME, sizeof(std::string *), sdesktop::cdc_queue_len},
         {sdesktop::SEND_QUEUE_BUFFER_NAME, sizeof(std::string *), sdesktop::cdc_queue_object_size}});

    if (ret == false) {
        LOG_ERROR("!!! service-desktop InitHandler failed to initialize worker, service-desktop won't work");
        return sys::ReturnCodes::Failure;
    }
    else {
        desktopWorker->run();
    }
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
                msgToSend->updateStats.versionInformation = UpdateMuditaOS::getVersionInfoFromFile(file);
                bus.sendUnicast(msgToSend, app::name_desktop);
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

    settings->registerValueChange(updateos::settings::history,
                                  [this](const std::string &value) { updateOS->setInitialHistory(value); });

    return (sys::ReturnCodes::Success);
}

sys::ReturnCodes ServiceDesktop::DeinitHandler()
{
    desktopWorker->deinit();
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

void ServiceDesktop::storeHistory(const std::string &historyValue)
{
    settings->setValue(updateos::settings::history, historyValue);
}
