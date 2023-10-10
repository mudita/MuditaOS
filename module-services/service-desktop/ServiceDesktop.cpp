// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-desktop/ServiceDesktop.hpp>
#include <endpoints/EndpointFactory.hpp>
#include <endpoints/bluetooth/BluetoothMessagesHandler.hpp>
#include <service-appmgr/ServiceApplicationManagerName.hpp>
#include <service-evtmgr/EventManagerCommon.hpp>
#include <service-evtmgr/EVMessages.hpp>
#include <system/messages/TetheringStateRequest.hpp>
#include <Timers/TimerFactory.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <service-desktop/endpoints/include/endpoints/filesystem/FileOperations.hpp>

ServiceDesktop::ServiceDesktop(const std::filesystem::path &mtpRootPath)
    : sys::Service(service::name::service_desktop, "", sdesktop::serviceStackSize),
      btMsgHandler(std::make_unique<sdesktop::bluetooth::BluetoothMessagesHandler>(this)),
      connectionActiveTimer{sys::TimerFactory::createSingleShotTimer(
          this,
          sdesktop::connectionActiveTimerName,
          sdesktop::connectionActiveTimerDelayMs,
          [this](sys::Timer & /*timer*/) { outboxNotifications.clearNotifications(); })},
      mtpRootPath{mtpRootPath}
{
    bus.channels.push_back(sys::BusChannel::PhoneLockChanges);
    bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
    bus.channels.push_back(sys::BusChannel::USBNotifications);
}

ServiceDesktop::~ServiceDesktop()
{
}

sys::ReturnCodes ServiceDesktop::InitHandler()
{
    settings = std::make_unique<settings::Settings>();
    settings->init(service::ServiceProxy(shared_from_this()));
    usbSecurityModel = std::make_unique<sdesktop::USBSecurityModel>(this, settings.get());

    connectHandler<db::NotificationMessage>();
    connectHandler<locks::UnlockedPhone>();
    connectHandler<locks::LockedPhone>();
    connectHandler<locks::NextPhoneUnlockAttemptLockTime>();
    connectHandler<message::bluetooth::ResponseStatus>();
    connectHandler<message::bluetooth::ResponseBondedDevices>();
    connectHandler<message::bluetooth::ResponseVisibleDevices>();
    connectHandler<sdesktop::developerMode::DeveloperModeRequest>();
    connectHandler<sdesktop::SyncMessage>();
    connectHandler<sdesktop::FactoryMessage>();
    connectHandler<sdesktop::usb::USBConfigured>();
    connectHandler<sdesktop::usb::USBDisconnected>();
    connectHandler<sevm::USBPlugEvent>();

    checkChargingCondition();

    LOG_INFO("Initialized");
    return sys::ReturnCodes::Success;
}

sys::ReturnCodes ServiceDesktop::DeinitHandler()
{
    LOG_INFO("Deinitialized");
    return usbWorkerDeinit();
}

sys::ReturnCodes ServiceDesktop::SwitchPowerModeHandler(const sys::ServicePowerMode /*mode*/)
{
    return sys::ReturnCodes::Success;
}

sys::MessagePointer ServiceDesktop::DataReceivedHandler(sys::DataMessage * /*msg*/, sys::ResponseMessage *resp)
{
    auto response = std::make_shared<sys::ResponseMessage>();
    if (resp == nullptr) {
        return response;
    }
    if (resp->responseTo != MessageType::DBQuery) {
        return response;
    }
    if (auto queryResponse = dynamic_cast<db::QueryResponse *>(resp)) {
        auto result = queryResponse->getResult();
        if (result == nullptr) {
            LOG_ERROR("Wrong result: nullptr!");
            return response;
        }
        if (result->hasListener()) {
            LOG_DEBUG("Handling result");
            result->handle();
        }
    }

    return response;
}

void ServiceDesktop::prepareSyncData()
{
    syncStatus.state   = Sync::OperationState::Stopped;
    syncStatus.tempDir = purefs::dir::getTemporaryPath() / "sync";
}

auto ServiceDesktop::requestLogsFlush() -> void
{
    int response = 0;
    auto ret     = bus.sendUnicastSync(
        std::make_shared<sevm::FlushLogsRequest>(), service::name::evt_manager, DefaultLogFlushTimeoutInMs);

    if (ret.first == sys::ReturnCodes::Success) {
        auto responseMsg = std::dynamic_pointer_cast<sevm::FlushLogsResponse>(ret.second);
        if ((responseMsg != nullptr) && responseMsg->retCode) {
            response = responseMsg->data;

            LOG_DEBUG("Response data: %d", response);
        }
    }
    if (ret.first == sys::ReturnCodes::Failure || response < 0) {
        throw std::runtime_error("Logs flush failed");
    }
}

auto ServiceDesktop::getSerialNumber() const -> std::string
{
    return settings->getValue(sdesktop::pathFactoryDataSerial, settings::SettingsScope::Global);
}

auto ServiceDesktop::getCaseColour() const -> std::string
{
    return settings->getValue(sdesktop::pathFactoryDataCaseColour, settings::SettingsScope::Global);
}

auto ServiceDesktop::getDeviceToken() -> std::string
{
    auto tokenSeed = getDeviceUniqueId();

    if (tokenSeed.empty()) {
        LOG_DEBUG("Device unique id is empty, generating one...");
        generateDeviceUniqueId();
        tokenSeed = getDeviceUniqueId();
    }

    return tokenSeed;
}

auto ServiceDesktop::getNotificationEntries() const -> std::vector<Outbox::NotificationEntry>
{
    return outboxNotifications.getNotificationEntries();
}

void ServiceDesktop::removeNotificationEntries(const std::vector<std::uint32_t> &uidsOfNotificationsToBeRemoved)
{
    outboxNotifications.removeNotificationEntries(uidsOfNotificationsToBeRemoved);
}

auto ServiceDesktop::generateDeviceUniqueId() -> void
{
    const auto deviceUniqueId = utils::generateRandomId(sdesktop::deviceUniqueIdLength);
    LOG_SENSITIVE(LOGINFO, "Device unique id: %s", deviceUniqueId.c_str());
    setDeviceUniqueId(deviceUniqueId);
}

auto ServiceDesktop::getDeviceUniqueId() const -> std::string
{
    return settings->getValue(sdesktop::deviceUniqueIdName);
}

auto ServiceDesktop::setDeviceUniqueId(const std::string &token) -> void
{
    return settings->setValue(sdesktop::deviceUniqueIdName, token);
}

auto ServiceDesktop::usbWorkerInit() -> sys::ReturnCodes
{
    if (initialized) {
        return sys::ReturnCodes::Success;
    }
    auto serialNumber = getSerialNumber();
    auto caseColour   = getCaseColour();

    LOG_DEBUG("Initializing USB worker, serial number: %s, case colour: %s", serialNumber.c_str(), caseColour.c_str());

    desktopWorker = std::make_unique<WorkerDesktop>(this,
                                                    std::bind(&ServiceDesktop::restartConnectionActiveTimer, this),
                                                    *usbSecurityModel,
                                                    serialNumber,
                                                    caseColour,
                                                    mtpRootPath);

    initialized = desktopWorker->init(
        {{sdesktop::cdcReceiveQueueName, sdesktop::cdcReceiveQueueItemSize, sdesktop::cdcReceiveQueueLength},
         {sdesktop::cdcSendQueueName, sdesktop::cdcSendQueueItemSize, sdesktop::cdcSendQueueLength},
         {sdesktop::irqQueueName, sdesktop::irqQueueItemSize, sdesktop::irqQueueLength},
         {sdesktop::signallingQueueName, sdesktop::signallingQueueItemSize, sdesktop::signallingQueueLength}});

    if (!initialized) {
        LOG_FATAL("Failed to initialize USB worker, ServiceDesktop won't work!");
        return sys::ReturnCodes::Failure;
    }

    desktopWorker->run();
    return sys::ReturnCodes::Success;
}

auto ServiceDesktop::usbWorkerDeinit() -> sys::ReturnCodes
{
    if (!initialized) {
        return sys::ReturnCodes::Success;
    }

    LOG_DEBUG("Deinitializing USB worker");
    settings->deinit();
    desktopWorker->closeWorker();
    desktopWorker.reset();
    initialized     = false;
    isUsbConfigured = false;
    LOG_DEBUG("USB worker deinitialized");
    // It must be run after the worker is closed.
    cleanFileSystemEndpointUndeliveredTransfers();
    return sys::ReturnCodes::Success;
}

void ServiceDesktop::restartConnectionActiveTimer()
{
    connectionActiveTimer.restart(sdesktop::connectionActiveTimerDelayMs);
}

void ServiceDesktop::checkChargingCondition()
{
    if (Store::Battery::get().state == Store::Battery::State::Discharging) {
        usbWorkerDeinit();
    }
    else {
        usbWorkerInit();
        isPlugEventUnhandled = true;
    }
}

auto ServiceDesktop::handle(db::NotificationMessage *msg) -> std::shared_ptr<sys::Message>
{
    if (!connectionActiveTimer.isActive()) {
        return sys::MessageNone{};
    }
    auto notificationMessage = static_cast<db::NotificationMessage *>(msg);
    outboxNotifications.newNotificationHandler(notificationMessage);
    return sys::MessageNone{};
}

auto ServiceDesktop::handle(locks::UnlockedPhone * /*msg*/) -> std::shared_ptr<sys::Message>
{
    LOG_INFO("Phone unlocked.");
    usbSecurityModel->setPhoneUnlocked();

    if (isUsbConfigured && isPlugEventUnhandled) {
        bus.sendUnicast(std::make_shared<sys::TetheringStateRequest>(sys::phone_modes::Tethering::On),
                        service::name::system_manager);
        isPlugEventUnhandled = false;
        desktopWorker->notify(WorkerDesktop::Signal::unlockMTP);
    }

    return sys::MessageNone{};
}

auto ServiceDesktop::handle(locks::LockedPhone * /*msg*/) -> std::shared_ptr<sys::Message>
{
    LOG_INFO("Phone locked.");
    usbSecurityModel->setPhoneLocked();
    return sys::MessageNone{};
}

auto ServiceDesktop::handle(locks::NextPhoneUnlockAttemptLockTime *msg) -> std::shared_ptr<sys::Message>
{
    auto message = static_cast<locks::NextPhoneUnlockAttemptLockTime *>(msg);
    usbSecurityModel->updatePhoneLockTime(message->getTime());
    return sys::MessageNone{};
}

auto ServiceDesktop::handle(message::bluetooth::ResponseStatus *msg) -> std::shared_ptr<sys::Message>
{
    auto msgl = static_cast<message::bluetooth::ResponseStatus *>(msg);
    return btMsgHandler->handle(msgl);
}

auto ServiceDesktop::handle(message::bluetooth::ResponseBondedDevices *msg) -> std::shared_ptr<sys::Message>
{
    auto msgl = static_cast<message::bluetooth::ResponseBondedDevices *>(msg);
    return btMsgHandler->handle(msgl);
}

auto ServiceDesktop::handle(message::bluetooth::ResponseVisibleDevices *msg) -> std::shared_ptr<sys::Message>
{
    auto msgl = static_cast<message::bluetooth::ResponseVisibleDevices *>(msg);
    return btMsgHandler->handle(msgl);
}

auto ServiceDesktop::handle(sdesktop::developerMode::DeveloperModeRequest *msg) -> std::shared_ptr<sys::Message>
{
    auto request = static_cast<sdesktop::developerMode::DeveloperModeRequest *>(msg);
    if (request->event != nullptr) {
        request->event->send();
    }

    return sys::MessageNone{};
}

auto ServiceDesktop::handle(sdesktop::SyncMessage * /*msg*/) -> std::shared_ptr<sys::Message>
{
    syncStatus.state          = Sync::OperationState::Running;
    syncStatus.completionCode = Sync::PrepareSyncPackage(this, syncStatus.tempDir);

    if (syncStatus.completionCode == Sync::CompletionCode::Success) {
        LOG_INFO("Sync package preparation finished");
        syncStatus.state = Sync::OperationState::Finished;
    }
    else {
        LOG_ERROR("Sync package preparation failed");
        syncStatus.state = Sync::OperationState::Error;
    }

    return sys::MessageNone{};
}

auto ServiceDesktop::handle(sdesktop::FactoryMessage * /*msg*/) -> std::shared_ptr<sys::Message>
{
    LOG_DEBUG("ServiceDesktop: FactoryMessage received");
    sys::SystemManagerCommon::FactoryReset(this);

    return sys::MessageNone{};
}

auto ServiceDesktop::handle(sdesktop::usb::USBConfigured *msg) -> std::shared_ptr<sys::Message>
{
    isUsbConfigured = true;
    if (usbSecurityModel->isSecurityEnabled()) {
        LOG_INFO("Endpoint security enabled, requesting passcode");
        bus.sendUnicast(std::make_shared<locks::UnlockPhoneForMTP>(), service::name::appmgr);
    }
    else if (isPlugEventUnhandled) {
        bus.sendUnicast(std::make_shared<sys::TetheringStateRequest>(sys::phone_modes::Tethering::On),
                        service::name::system_manager);
        isPlugEventUnhandled = false;
        desktopWorker->notify(WorkerDesktop::Signal::unlockMTP);
    }

    return sys::MessageNone{};
}

auto ServiceDesktop::handle(sdesktop::usb::USBDisconnected * /*msg*/) -> std::shared_ptr<sys::Message>
{
    LOG_INFO("USB disconnected");
    if (usbSecurityModel->isSecurityEnabled()) {
        bus.sendUnicast(std::make_shared<locks::CancelUnlockPhone>(), service::name::appmgr);
    }
    bus.sendUnicast(std::make_shared<sys::TetheringStateRequest>(sys::phone_modes::Tethering::Off),
                    service::name::system_manager);

    return sys::MessageNone{};
}

auto ServiceDesktop::handle(sevm::USBPlugEvent *msg) -> std::shared_ptr<sys::Message>
{
    const auto message = static_cast<sevm::USBPlugEvent *>(msg);
    if (message->event == sevm::USBPlugEvent::Event::CablePlugged) {
        usbWorkerInit();
        isPlugEventUnhandled = true;
    }
    else {
        usbWorkerDeinit();
    }
    return sys::MessageNone{};
}

auto ServiceDesktop::getMtpPath() const noexcept -> std::filesystem::path
{
    return mtpRootPath;
}

auto ServiceDesktop::getOnboardingState() const -> sdesktop::endpoints::OnboardingState
{
    return static_cast<sdesktop::endpoints::OnboardingState>(utils::getNumericValue<int>(
        settings->getValue(settings::SystemProperties::onboardingDone, settings::SettingsScope::Global)));
}

void ServiceDesktop::cleanFileSystemEndpointUndeliveredTransfers()
{
    FileOperations::instance().cleanUpUndeliveredTransfers();
}
