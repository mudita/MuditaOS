// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-appmgr/messages/DOMRequest.hpp"
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
#include <json11.hpp>
#include <log.hpp>
#include <application-desktop/Constants.hpp>
#include <service-db/service-db/Settings.hpp>
#include <service-db/QueryMessage.hpp>
#include <purefs/filesystem_paths.hpp>
#include <module-sys/SystemManager/SystemManager.hpp>
#include <module-sys/Timers/TimerFactory.hpp>

#include <service-appmgr/model/ApplicationManager.hpp>
#include <module-services/service-db/agents/settings/SystemSettings.hpp>
#include <module-sys/SystemManager/Constants.hpp>
#include <module-sys/SystemManager/messages/TetheringStateRequest.hpp>
#include <endpoints/bluetooth/BluetoothMessagesHandler.hpp>

#include <sys/mount.h>
#include <sys/statvfs.h>

#include <ctime>
#include <cinttypes>
#include <filesystem>

namespace
{
    bool RemountFS(bool readOnly = false, std::string path = std::string(purefs::dir::getRootDiskPath()))
    {
        struct statvfs stat;
        if (statvfs(path.c_str(), &stat)) {
            LOG_ERROR("Failed reading statvfs!");
            return false;
        }
        auto flags = stat.f_flag;
        if ((readOnly && (flags & MS_RDONLY)) || (!readOnly && !(flags & MS_RDONLY))) {
            LOG_WARN("Filesystem is already mounted in requested mode!");
            return false;
        }
        if (readOnly) {
            LOG_INFO("Remounting filesystem RO");
            flags |= MS_RDONLY;
        }
        else {
            LOG_INFO("Remounting filesystem R/W");
            flags &= ~MS_RDONLY;
        }
        return !mount(NULL, path.c_str(), NULL, flags | MS_REMOUNT, NULL);
    }
} // namespace

ServiceDesktop::ServiceDesktop()
    : sys::Service(service::name::service_desktop, "", sdesktop::service_stack),
      btMsgHandler(std::make_unique<sdesktop::bluetooth::BluetoothMessagesHandler>(this))
{
    LOG_INFO("[ServiceDesktop] Initializing");
    bus.channels.push_back(sys::BusChannel::PhoneLockChanges);

    updateOS         = std::make_unique<UpdateMuditaOS>(this);
}

ServiceDesktop::~ServiceDesktop()
{
    LOG_INFO("[ServiceDesktop] Cleaning resources");
}

auto ServiceDesktop::getSerialNumber() const -> std::string
{
    return settings->getValue(std::string("factory_data/serial"), settings::SettingsScope::Global);
}

sys::ReturnCodes ServiceDesktop::InitHandler()
{

    settings = std::make_unique<settings::Settings>();
    settings->init(service::ServiceProxy(shared_from_this()));
    usbSecurityModel = std::make_unique<sdesktop::USBSecurityModel>(this, settings.get());

    auto serialNumber = getSerialNumber();

    LOG_DEBUG("Serial Number: %s", serialNumber.c_str());

    desktopWorker = std::make_unique<WorkerDesktop>(this, *usbSecurityModel.get(), serialNumber);

    const bool ret =
        desktopWorker->init({{sdesktop::RECEIVE_QUEUE_BUFFER_NAME, sizeof(std::string *), sdesktop::cdc_queue_len},
                             {sdesktop::SEND_QUEUE_BUFFER_NAME, sizeof(std::string *), sdesktop::cdc_queue_object_size},
                             {sdesktop::IRQ_QUEUE_BUFFER_NAME, 1, sdesktop::irq_queue_object_size}});

    if (ret == false) {
        LOG_ERROR("!!! service-desktop InitHandler failed to initialize worker, service-desktop won't work");
        return sys::ReturnCodes::Failure;
    }
    else {
        desktopWorker->run();
    }

    transferTimer =
        sys::TimerFactory::createPeriodicTimer(this,
                                               "WorkerDesktop file upload",
                                               std::chrono::milliseconds{sdesktop::file_transfer_timeout},
                                               [this](sys::Timer &) { desktopWorker->cancelTransferOnTimeout(); });

    connect(sdesktop::developerMode::DeveloperModeRequest(), [&](sys::Message *msg) {
        auto request = static_cast<sdesktop::developerMode::DeveloperModeRequest *>(msg);
        if (request->event != nullptr) {
            request->event->send();
        }

        return sys::MessageNone{};
    });

    connect(sdesktop::BackupMessage(), [&](sys::Message *msg) {
        sdesktop::BackupMessage *backupMessage = dynamic_cast<sdesktop::BackupMessage *>(msg);
        if (backupMessage != nullptr) {
            RemountFS();

            backupRestoreStatus.state = OperationState::Running;
            backupRestoreStatus.lastOperationResult =
                BackupRestore::BackupUserFiles(this, backupRestoreStatus.backupTempDir);
            backupRestoreStatus.location =
                (purefs::dir::getBackupOSPath() / backupRestoreStatus.task).replace_extension(purefs::extension::tar);
            backupRestoreStatus.state = OperationState::Stopped;
        }
        return sys::MessageNone{};
    });

    connect(sdesktop::RestoreMessage(), [&](sys::Message *msg) {
        sdesktop::RestoreMessage *restoreMessage = dynamic_cast<sdesktop::RestoreMessage *>(msg);
        if (restoreMessage != nullptr) {
            RemountFS();
            backupRestoreStatus.state = OperationState::Running;
            backupRestoreStatus.lastOperationResult =
                BackupRestore::RestoreUserFiles(this, backupRestoreStatus.location);

            backupRestoreStatus.state = OperationState::Stopped;
            if (backupRestoreStatus.lastOperationResult == true) {
                sys::SystemManager::Reboot(this);
            }
            else {
                LOG_ERROR("Restore failed");
            }
        }
        return sys::MessageNone{};
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
        return sys::MessageNone{};
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
            LOG_DEBUG("ServiceDesktop::DataReceivedHandler file:%s uuid:%" PRIu32 "",
                      updateOsMsg->updateStats.updateFile.c_str(),
                      updateOsMsg->updateStats.uuid);

            if (updateOS->setUpdateFile(purefs::dir::getUpdatesOSPath(), updateOsMsg->updateStats.updateFile) ==
                updateos::UpdateError::NoError) {
                RemountFS();
                // Same possible issue as with FactoryReset::Run()
                updateOS->runUpdate();
            }
        }

        return sys::MessageNone{};
    });

    connect(sdesktop::transfer::TransferTimerState(), [&](sys::Message *msg) {
        sdesktop::transfer::TransferTimerState *timerStateMsg =
            dynamic_cast<sdesktop::transfer::TransferTimerState *>(msg);

        if (timerStateMsg != nullptr && timerStateMsg->messageType == MessageType::TransferTimer) {
            switch (timerStateMsg->req) {
            case sdesktop::transfer::TransferTimerState::Start:
                [[fallthrough]];
            case sdesktop::transfer::TransferTimerState::Reload:
                transferTimer.start();
                break;
            case sdesktop::transfer::TransferTimerState::Stop:
                transferTimer.stop();
                break;
            case sdesktop::transfer::TransferTimerState::None:
                [[fallthrough]];
            default:
                LOG_DEBUG("ServiceDesktop::SetTransferTimerState unhandled req:%u", timerStateMsg->req);
                break;
            }
        }
        return sys::MessageNone{};
    });

    connect(sdesktop::usb::USBConfigured(), [&](sys::Message *msg) {
        auto message = static_cast<sdesktop::usb::USBConfigured *>(msg);
        if (message->getConfigurationType() == sdesktop::usb::USBConfigurationType::firstConfiguration) {
            bus.sendUnicast(std::make_shared<sys::TetheringStateRequest>(sys::phone_modes::Tethering::On),
                            service::name::system_manager);
        }

        if (usbSecurityModel->isSecurityEnabled()) {
            LOG_INFO("Endpoint security enabled, requesting passcode");

            bus.sendUnicast(std::make_shared<locks::UnlockPhone>(), app::manager::ApplicationManager::ServiceName);
        }

        return sys::MessageNone{};
    });

    connect(sdesktop::usb::USBDisconnected(), [&](sys::Message *msg) {
        LOG_INFO("USB disconnected");
        if (usbSecurityModel->isSecurityEnabled()) {
            bus.sendUnicast(std::make_shared<locks::CancelUnlockPhone>(),
                            app::manager::ApplicationManager::ServiceName);
        }
        bus.sendUnicast(std::make_shared<sys::TetheringStateRequest>(sys::phone_modes::Tethering::Off),
                        service::name::system_manager);
        return sys::MessageNone{};
    });

    connect(typeid(locks::UnlockedPhone), [&](sys::Message *msg) {
        LOG_INFO("Phone unlocked.");
        usbSecurityModel->setPhoneUnlocked();
        return sys::MessageNone{};
    });

    connect(typeid(locks::LockedPhone), [&](sys::Message *msg) {
        LOG_INFO("Phone locked.");
        usbSecurityModel->setPhoneLocked();
        return sys::MessageNone{};
    });

    connect(typeid(message::bluetooth::ResponseStatus), [&](sys::Message *msg) {
        auto msgl = static_cast<message::bluetooth::ResponseStatus *>(msg);
        return btMsgHandler->handle(msgl);
    });

    connect(typeid(message::bluetooth::ResponseBondedDevices), [&](sys::Message *msg) {
        auto msgl = static_cast<message::bluetooth::ResponseBondedDevices *>(msg);
        return btMsgHandler->handle(msgl);
    });

    connect(typeid(message::bluetooth::ResponseVisibleDevices), [&](sys::Message *msg) {
        auto msgl = static_cast<message::bluetooth::ResponseVisibleDevices *>(msg);
        return btMsgHandler->handle(msgl);
    });
    settings->registerValueChange(updateos::settings::history,
                                  [this](const std::string &value) { updateOS->setInitialHistory(value); });

    return (sys::ReturnCodes::Success);
}

sys::ReturnCodes ServiceDesktop::DeinitHandler()
{
    LOG_ERROR(".. deinit ..");
    settings->deinit();
    desktopWorker->deinit();
    return sys::ReturnCodes::Success;
}

void ServiceDesktop::ProcessCloseReason(sys::CloseReason closeReason)
{
    LOG_ERROR(".. close with reason ..");
    settings->deinit();
    desktopWorker->deinit();
    sendCloseReadyMessage(this);
}

sys::ReturnCodes ServiceDesktop::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
{
    return sys::ReturnCodes::Success;
}

sys::MessagePointer ServiceDesktop::DataReceivedHandler(sys::DataMessage *msg, sys::ResponseMessage *resp)
{
    if (resp != nullptr) {
        if (resp->responseTo == MessageType::DBQuery) {
            if (auto queryResponse = dynamic_cast<db::QueryResponse *>(resp)) {
                auto result = queryResponse->getResult();
                if (result != nullptr) {
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
    backupRestoreStatus.operation = ServiceDesktop::Operation::Backup;
    backupRestoreStatus.task          = std::to_string(static_cast<uint32_t>(std::time(nullptr)));
    backupRestoreStatus.state     = OperationState::Stopped;
    backupRestoreStatus.backupTempDir = purefs::dir::getTemporaryPath() / backupRestoreStatus.task;
}

void ServiceDesktop::prepareRestoreData(const std::filesystem::path &restoreLocation)
{
    backupRestoreStatus.operation = ServiceDesktop::Operation::Restore;
    backupRestoreStatus.location  = purefs::dir::getBackupOSPath() / restoreLocation;
    backupRestoreStatus.state     = OperationState::Stopped;
    backupRestoreStatus.task      = restoreLocation.filename();
}
