// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-appmgr/messages/DOMRequest.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <service-desktop/ServiceDesktop.hpp>
#include <service-desktop/WorkerDesktop.hpp>
#include <service-cellular/CellularMessage.hpp>
#include <service-desktop/BackupRestore.hpp>
#include <endpoints/EndpointFactory.hpp>
#include <endpoints/bluetooth/BluetoothMessagesHandler.hpp>

#include <Common/Query.hpp>
#include <MessageType.hpp>
#include <Service/Worker.hpp>
#include <json11.hpp>
#include <log/log.hpp>
#include <application-desktop/Constants.hpp>
#include <locks/data/PhoneLockMessages.hpp>
#include <service-appmgr/Constants.hpp>
#include <service-db/QueryMessage.hpp>
#include <service-evtmgr/EventManagerCommon.hpp>
#include <service-evtmgr/EVMessages.hpp>
#include <purefs/filesystem_paths.hpp>
#include <SystemManager/SystemManagerCommon.hpp>
#include <Timers/TimerFactory.hpp>

#include <system/Constants.hpp>
#include <system/messages/TetheringStateRequest.hpp>

#include <sys/mount.h>
#include <sys/statvfs.h>

#include <ctime>
#include <cinttypes>
#include <filesystem>

ServiceDesktop::ServiceDesktop()
    : sys::Service(service::name::service_desktop, "", sdesktop::service_stack),
      btMsgHandler(std::make_unique<sdesktop::bluetooth::BluetoothMessagesHandler>(this))
{
    LOG_INFO("[ServiceDesktop] Initializing");
    bus.channels.push_back(sys::BusChannel::PhoneLockChanges);
}

ServiceDesktop::~ServiceDesktop()
{
    LOG_INFO("[ServiceDesktop] Cleaning resources");
}

auto ServiceDesktop::getSerialNumber() const -> std::string
{
    return settings->getValue(std::string("factory_data/serial"), settings::SettingsScope::Global);
}

auto ServiceDesktop::getCaseColour() const -> std::string
{
    return settings->getValue(std::string("factory_data/case_colour"), settings::SettingsScope::Global);
}

auto ServiceDesktop::requestLogsFlush() -> void
{
    int response = 0;
    auto ret     = bus.sendUnicastSync(
        std::make_shared<sevm::FlushLogsRequest>(), service::name::evt_manager, DefaultLogFlushTimeoutInMs);

    if (ret.first == sys::ReturnCodes::Success) {
        auto responseMsg = std::dynamic_pointer_cast<sevm::FlushLogsResponse>(ret.second);
        if ((responseMsg != nullptr) && (responseMsg->retCode == true)) {
            response = responseMsg->data;

            LOG_DEBUG("Respone data: %d", response);
        }
    }
    if (ret.first == sys::ReturnCodes::Failure || response < 0) {
        throw std::runtime_error("Logs flush failed");
    }
}

sys::ReturnCodes ServiceDesktop::InitHandler()
{

    settings = std::make_unique<settings::Settings>();
    settings->init(service::ServiceProxy(shared_from_this()));
    usbSecurityModel = std::make_unique<sdesktop::USBSecurityModel>(this, settings.get());

    auto serialNumber = getSerialNumber();

    LOG_DEBUG("Serial Number: %s", serialNumber.c_str());

    desktopWorker = std::make_unique<WorkerDesktop>(this, *usbSecurityModel.get(), serialNumber);

    initialized =
        desktopWorker->init({{sdesktop::RECEIVE_QUEUE_BUFFER_NAME, sizeof(std::string *), sdesktop::cdc_queue_len},
                             {sdesktop::SEND_QUEUE_BUFFER_NAME, sizeof(std::string *), sdesktop::cdc_queue_object_size},
                             {sdesktop::IRQ_QUEUE_BUFFER_NAME, 1, sdesktop::irq_queue_object_size}});

    if (initialized == false) {
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

        return sys::MessageNone{};
    });

    connect(sdesktop::BackupMessage(), [&](sys::Message *msg) {
        sdesktop::BackupMessage *backupMessage = dynamic_cast<sdesktop::BackupMessage *>(msg);
        if (backupMessage != nullptr) {
            backupRestoreStatus.state = BackupRestore::OperationState::Running;
            backupRestoreStatus.completionCode =
                BackupRestore::BackupUserFiles(this, backupRestoreStatus.backupTempDir);
            backupRestoreStatus.location = backupRestoreStatus.taskId;

            if (backupRestoreStatus.completionCode == BackupRestore::CompletionCode::Success) {
                LOG_INFO("Backup finished");
                backupRestoreStatus.state = BackupRestore::OperationState::Finished;
            }
            else {
                LOG_ERROR("Backup failed");
                backupRestoreStatus.state = BackupRestore::OperationState::Error;
            }
        }
        return sys::MessageNone{};
    });

    connect(sdesktop::RestoreMessage(), [&](sys::Message *msg) {
        sdesktop::RestoreMessage *restoreMessage = dynamic_cast<sdesktop::RestoreMessage *>(msg);
        if (restoreMessage != nullptr) {
            backupRestoreStatus.state          = BackupRestore::OperationState::Running;
            backupRestoreStatus.completionCode = BackupRestore::RestoreUserFiles(this, backupRestoreStatus.location);

            if (backupRestoreStatus.completionCode == BackupRestore::CompletionCode::Success) {
                LOG_DEBUG("Restore finished");
                backupRestoreStatus.state = BackupRestore::OperationState::Finished;
                sys::SystemManagerCommon::Reboot(this);
            }
            else {
                LOG_ERROR("Restore failed");
                backupRestoreStatus.state = BackupRestore::OperationState::Error;
            }
        }
        return sys::MessageNone{};
    });

    connect(sdesktop::FactoryMessage(), [&](sys::Message *msg) {
        auto *factoryMessage = dynamic_cast<sdesktop::FactoryMessage *>(msg);
        if (factoryMessage != nullptr) {
            LOG_DEBUG("ServiceDesktop: FactoryMessage received");
            sys::SystemManagerCommon::FactoryReset(this);
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

            bus.sendUnicast(std::make_shared<locks::UnlockPhone>(), service::name::appmgr);
        }

        return sys::MessageNone{};
    });

    connect(sdesktop::usb::USBDisconnected(), [&](sys::Message *msg) {
        LOG_INFO("USB disconnected");
        if (usbSecurityModel->isSecurityEnabled()) {
            bus.sendUnicast(std::make_shared<locks::CancelUnlockPhone>(), service::name::appmgr);
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

    connect(typeid(locks::NextPhoneUnlockAttemptLockTime), [&](sys::Message *msg) {
        auto message = static_cast<locks::NextPhoneUnlockAttemptLockTime *>(msg);
        usbSecurityModel->updatePhoneLockTime(message->getTime());
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

    return (sys::ReturnCodes::Success);
}

sys::ReturnCodes ServiceDesktop::DeinitHandler()
{
    LOG_DEBUG(".. deinit ..");
    if (initialized) {
        settings->deinit();
        desktopWorker->deinit();
        initialized = false;
    }
    return sys::ReturnCodes::Success;
}

void ServiceDesktop::ProcessCloseReason(sys::CloseReason closeReason)
{
    LOG_DEBUG(".. close with reason ..");
    DeinitHandler();
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

std::string ServiceDesktop::prepareBackupFilename()
{
    std::array<char, 64> backupFileName;
    std::time_t now;
    std::time(&now);
    std::strftime(backupFileName.data(), backupFileName.size(), "%FT%OH%OM%OSZ", std::localtime(&now));

    return std::string(backupFileName.data());
}

void ServiceDesktop::prepareBackupData()
{
    backupRestoreStatus.operation     = BackupRestore::Operation::Backup;
    backupRestoreStatus.taskId        = prepareBackupFilename();
    backupRestoreStatus.state         = BackupRestore::OperationState::Stopped;
    backupRestoreStatus.backupTempDir = purefs::dir::getTemporaryPath() / backupRestoreStatus.taskId;
}

void ServiceDesktop::prepareRestoreData(const std::filesystem::path &restoreLocation)
{
    backupRestoreStatus.operation = BackupRestore::Operation::Restore;
    backupRestoreStatus.taskId    = restoreLocation.filename();
    backupRestoreStatus.state     = BackupRestore::OperationState::Stopped;
    backupRestoreStatus.location  = purefs::dir::getBackupOSPath() / backupRestoreStatus.taskId;
}
