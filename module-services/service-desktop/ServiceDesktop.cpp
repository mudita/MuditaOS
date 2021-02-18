// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-appmgr/service-appmgr/messages/DOMRequest.hpp"
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
#include <purefs/filesystem_paths.hpp>
#include <module-sys/SystemManager/SystemManager.hpp>

#include <cinttypes>
#include <filesystem>

#include <purefs/filesystem_paths.hpp>
#include <sys/mount.h>
#include <sys/statvfs.h>

namespace
{
    auto RemountFS(bool readOnly = false, std::string path = std::string(purefs::dir::getRootDiskPath()))
    {
        struct statvfs stat;
        if (auto ret = statvfs(path.c_str(), &stat))
            return ret;
        auto flags = stat.f_flag;
        if (readOnly)
            flags |= MS_RDONLY;
        else
            flags &= ~MS_RDONLY;
        return mount(NULL, path.c_str(), NULL, flags | MS_REMOUNT, NULL);
    }
} // namespace

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
    desktopWorker  = std::make_unique<WorkerDesktop>(this);
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

    transferTimer = std::make_unique<sys::Timer>("WorkerDesktop file upload", this, sdesktop::file_transfer_timeout);
    transferTimer->connect([&](sys::Timer &) { desktopWorker->cancelTransferOnTimeout(); });

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
            backupStatus.state = BackupRestore::BackupUserFiles(this, backupStatus.backupTempDir);
            backupStatus.location =
                (purefs::dir::getBackupOSPath() / backupStatus.task).replace_extension(purefs::extension::tar);
        }
        return std::make_shared<sys::ResponseMessage>();
    });

    connect(sdesktop::RestoreMessage(), [&](sys::Message *msg) {
        sdesktop::RestoreMessage *restoreMessage = dynamic_cast<sdesktop::RestoreMessage *>(msg);
        if (restoreMessage != nullptr) {
            RemountFS();
            BackupRestore::RestoreUserFiles(this);
        }
        return std::make_shared<sys::ResponseMessage>();
    });

    connect(sdesktop::FactoryMessage(), [&](sys::Message *msg) {
        auto *factoryMessage = dynamic_cast<sdesktop::FactoryMessage *>(msg);
        if (factoryMessage != nullptr) {
            LOG_DEBUG("ServiceDesktop: FactoryMessage received");
            RemountFS();
            // Factory reset calls SystemManager::Reboot(), but
            // there is no umount() in SystemManager::CloseSystemHandler() -
            // this might theoretically cause filesystem corruption
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

            if (updateOS->setUpdateFile(purefs::dir::getUpdatesOSPath(), updateOsMsg->updateStats.updateFile) ==
                updateos::UpdateError::NoError) {
                RemountFS();
                // Same possible issue as with FactoryReset::Run()
                updateOS->runUpdate();
            }
        }
        return std::make_shared<sys::ResponseMessage>();
    });

    connect(sdesktop::transfer::TransferTimerState(), [&](sys::Message *msg) {
        sdesktop::transfer::TransferTimerState *timerStateMsg =
            dynamic_cast<sdesktop::transfer::TransferTimerState *>(msg);

        if (timerStateMsg != nullptr && timerStateMsg->messageType == MessageType::TransferTimer) {
            switch (timerStateMsg->req) {
            case sdesktop::transfer::TransferTimerState::Start:
                transferTimer->start();
                break;
            case sdesktop::transfer::TransferTimerState::Reload:
                transferTimer->reload();
                break;
            case sdesktop::transfer::TransferTimerState::Stop:
                transferTimer->stop();
                break;
            case sdesktop::transfer::TransferTimerState::None:
                [[fallthrough]];
            default:
                LOG_DEBUG("ServiceDesktop::SetTransferTimerState unhandled req:%u", timerStateMsg->req);
                break;
            }
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

void ServiceDesktop::prepareBackupData()
{
    backupStatus.task          = std::to_string(static_cast<uint32_t>(utils::time::getCurrentTimestamp().getTime()));
    backupStatus.state         = false;
    backupStatus.backupTempDir = purefs::dir::getTemporaryPath() / backupStatus.task;
}
