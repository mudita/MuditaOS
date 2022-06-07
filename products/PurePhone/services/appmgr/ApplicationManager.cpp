// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <appmgr/ApplicationManager.hpp>

#include <apps-common/popups/data/BluetoothModeParams.hpp>
#include <application-desktop/ApplicationDesktop.hpp>
#include <application-onboarding/ApplicationOnBoarding.hpp>
#include <apps-common/popups/data/PhoneModeParams.hpp>
#include <apps-common/actions/AlarmClockStatusChangeParams.hpp>
#include <module-db/queries/notifications/QueryNotificationsGetAll.hpp>
#include <system/messages/TetheringQuestionRequest.hpp>
#include <Timers/TimerFactory.hpp>
#include <service-appmgr/Constants.hpp>
#include <service-appmgr/messages/AutoLockRequests.hpp>
#include <service-appmgr/messages/GetAllNotificationsRequest.hpp>
#include <service-appmgr/messages/GetWallpaperOptionRequest.hpp>
#include <service-bluetooth/messages/BluetoothModeChanged.hpp>
#include <service-cellular/CellularMessage.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <service-desktop/Constants.hpp>
#include <service-evtmgr/EVMessages.hpp>
#include <service-evtmgr/Constants.hpp>
#include <service-evtmgr/torch.hpp>
#include <sys/messages/TetheringPhoneModeChangeProhibitedMessage.hpp>
#include <service-time/include/service-time/AlarmMessage.hpp>
#include <service-time/include/service-time/AlarmServiceAPI.hpp>

namespace app::manager
{
    namespace
    {
        constexpr auto autoLockTimerName = "AutoLockTimer";
    } // namespace

    ApplicationManager::ApplicationManager(const ApplicationName &serviceName,
                                           std::vector<std::unique_ptr<app::ApplicationLauncher>> &&launchers,
                                           const ApplicationName &_rootApplicationName)
        : ApplicationManagerCommon(serviceName, std::move(launchers), _rootApplicationName),
          phoneModeObserver(std::make_shared<sys::phone_modes::Observer>()),
          phoneLockHandler(locks::PhoneLockHandler(this, settings)),
          simLockHandler(this), notificationsConfig{phoneModeObserver, settings, phoneLockHandler},
          notificationsHandler{this, notificationsConfig}, notificationProvider{this, notificationsConfig}
    {
        autoLockTimer = sys::TimerFactory::createSingleShotTimer(
            this, autoLockTimerName, sys::timer::InfiniteTimeout, [this](sys::Timer &) { onPhoneLocked(); });

        bus.channels.push_back(sys::BusChannel::BluetoothModeChanges);
        bus.channels.push_back(sys::BusChannel::BluetoothNotifications);
        bus.channels.push_back(sys::BusChannel::PhoneModeChanges);
        bus.channels.push_back(sys::BusChannel::PhoneLockChanges);
        bus.channels.push_back(sys::BusChannel::ServiceCellularNotifications);
        registerMessageHandlers();
    }

    sys::ReturnCodes ApplicationManager::InitHandler()
    {
        ApplicationManagerCommon::InitHandler();

        startBackgroundApplications();

        phoneLockHandler.enablePhoneLock((utils::getNumericValue<bool>(
            settings->getValue(settings::SystemProperties::lockScreenPasscodeIsOn, settings::SettingsScope::Global))));

        phoneLockHandler.setPhoneLockHash(
            settings->getValue(settings::SystemProperties::lockPassHash, settings::SettingsScope::Global));

        phoneLockHandler.setPhoneLockTime(utils::getNumericValue<time_t>(
            settings->getValue(settings::SystemProperties::unlockLockTime, settings::SettingsScope::Global)));

        phoneLockHandler.setNextUnlockAttemptLockTime(utils::getNumericValue<time_t>(
            settings->getValue(settings::SystemProperties::unlockAttemptLockTime, settings::SettingsScope::Global)));

        phoneLockHandler.setNoLockTimeAttemptsLeft(utils::getNumericValue<unsigned int>(
            settings->getValue(settings::SystemProperties::noLockTimeAttemptsLeft, settings::SettingsScope::Global)));

        wallpaperModel.setWallpaper(static_cast<gui::WallpaperOption>(utils::getNumericValue<unsigned int>(
            settings->getValue(settings::Wallpaper::option, settings::SettingsScope::Global))));

        settings->registerValueChange(
            settings::SystemProperties::lockScreenPasscodeIsOn,
            [this](const std::string &value) { phoneLockHandler.enablePhoneLock(utils::getNumericValue<bool>(value)); },
            settings::SettingsScope::Global);

        settings->registerValueChange(
            settings::SystemProperties::lockPassHash,
            [this](const std::string &value) { phoneLockHandler.setPhoneLockHash(value); },
            settings::SettingsScope::Global);

        settings->registerValueChange(
            ::settings::KeypadLight::state,
            [this](const std::string &value) {
                const auto keypadLightState =
                    static_cast<bsp::keypad_backlight::State>(utils::getNumericValue<int>(value));
                processKeypadBacklightState(keypadLightState);
            },
            ::settings::SettingsScope::Global);

        settings->registerValueChange(
            settings::SystemProperties::autoLockTimeInSec,
            [this](std::string value) { lockTimeChanged(std::move(value)); },
            settings::SettingsScope::Global);

        settings->registerValueChange(
            settings::Wallpaper::option,
            [this](std::string value) {
                wallpaperModel.setWallpaper(
                    static_cast<gui::WallpaperOption>(utils::getNumericValue<unsigned int>(value)));
            },
            settings::SettingsScope::Global);

        return sys::ReturnCodes::Success;
    }

    void ApplicationManager::processKeypadBacklightState(bsp::keypad_backlight::State keypadLightState)
    {
        auto action = bsp::keypad_backlight::Action::turnOff;
        switch (keypadLightState) {
        case bsp::keypad_backlight::State::activeMode:
            action = bsp::keypad_backlight::Action::turnOnActiveMode;
            break;
        case bsp::keypad_backlight::State::off:
            action = bsp::keypad_backlight::Action::turnOff;
            break;
        }
        bus.sendUnicast(std::make_shared<sevm::KeypadBacklightMessage>(action), service::name::evt_manager);
    }

    void ApplicationManager::startBackgroundApplications()
    {
        runAppsInBackground();
    }

    void ApplicationManager::registerMessageHandlers()
    {
        ApplicationManagerCommon::registerMessageHandlers();

        phoneModeObserver->connect(this);
        phoneModeObserver->subscribe([this](sys::phone_modes::PhoneMode phoneMode) {
            handlePhoneModeChanged(phoneMode);
            actionsRegistry.enqueue(
                ActionEntry{actions::ShowPopup, std::make_unique<gui::PhoneModePopupRequestParams>(phoneMode)});
        });

        phoneModeObserver->subscribe(
            [this](sys::phone_modes::Tethering tethering) { handleTetheringChanged(tethering); });

        notificationsHandler.registerMessageHandlers();

        connect(typeid(PreventBlockingRequest), [this]([[maybe_unused]] sys::Message *msg) {
            if (!phoneLockHandler.isPhoneLocked()) {
                autoLockTimer.start();
            }
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(GetAllNotificationsRequest), [&](sys::Message *request) {
            notificationProvider.requestNotSeenNotifications();
            notificationProvider.send();
            return sys::msgHandled();
        });
        connect(typeid(GetWallpaperOptionRequest), [&](sys::Message *request) {
            return std::make_shared<app::manager::GetWallpaperOptionResponse>(wallpaperModel.getWallpaper());
        });
        connect(typeid(db::NotificationMessage), [&](sys::Message *msg) {
            auto msgl = static_cast<db::NotificationMessage *>(msg);
            notificationProvider.handle(msgl);
            return sys::msgHandled();
        });
        connect(typeid(db::QueryResponse), [&](sys::Message *msg) {
            auto response = static_cast<db::QueryResponse *>(msg);
            handleDBResponse(response);
            return sys::msgHandled();
        });

        // PhoneLock connects
        connect(typeid(locks::LockPhone),
                [&](sys::Message *request) -> sys::MessagePointer { return phoneLockHandler.handleLockRequest(); });
        connect(typeid(locks::UnlockPhone),
                [&](sys::Message *request) -> sys::MessagePointer { return phoneLockHandler.handleUnlockRequest(); });
        connect(typeid(locks::CancelUnlockPhone), [&](sys::Message *request) -> sys::MessagePointer {
            return phoneLockHandler.handleUnlockCancelRequest();
        });
        connect(typeid(locks::UnLockPhoneInput), [&](sys::Message *request) -> sys::MessagePointer {
            auto data = static_cast<locks::UnLockPhoneInput *>(request);
            return phoneLockHandler.handlePhoneLockInput(data->getInputData());
        });
        connect(typeid(locks::EnablePhoneLock),
                [&](sys::Message *request) -> sys::MessagePointer { return phoneLockHandler.handleEnablePhoneLock(); });
        connect(typeid(locks::DisablePhoneLock), [&](sys::Message *request) -> sys::MessagePointer {
            return phoneLockHandler.handleDisablePhoneLock();
        });
        connect(typeid(locks::UnlockedPhone), [&](sys::Message *request) -> sys::MessagePointer {
            autoLockTimer.start();
            return simLockHandler.releaseSimUnlockBlockOnLockedPhone();
        });
        connect(typeid(locks::ChangePhoneLock),
                [&](sys::Message *request) -> sys::MessagePointer { return phoneLockHandler.handleChangePhoneLock(); });
        connect(typeid(locks::SetPhoneLock),
                [&](sys::Message *request) -> sys::MessagePointer { return phoneLockHandler.handleSetPhoneLock(); });
        connect(typeid(locks::SkipSetPhoneLock), [&](sys::Message *request) -> sys::MessagePointer {
            return phoneLockHandler.handleSkipSetPhoneLock();
        });
        connect(typeid(locks::PhoneLockTimeUpdate), [&](sys::Message *request) -> sys::MessagePointer {
            auto req = static_cast<locks::PhoneLockTimeUpdate *>(request);
            notificationProvider.handle(req);
            return sys::msgHandled();
        });
        connect(typeid(SetAutoLockTimeoutRequest), [&](sys::Message *request) -> sys::MessagePointer {
            auto req = static_cast<SetAutoLockTimeoutRequest *>(request);
            return handleAutoLockSetRequest(req);
        });
        connect(typeid(locks::ExternalUnLockPhone), [&](sys::Message *request) -> sys::MessagePointer {
            auto data = static_cast<locks::ExternalUnLockPhone *>(request);
            return phoneLockHandler.handleExternalUnlockRequest(data->getInputData());
        });
        connect(typeid(locks::ExternalPhoneLockAvailabilityChange), [&](sys::Message *request) -> sys::MessagePointer {
            auto data = static_cast<locks::ExternalPhoneLockAvailabilityChange *>(request);
            return phoneLockHandler.handleExternalAvailabilityChange(data->getAvailability());
        });

        // SimLock connects
        connect(typeid(cellular::msg::notification::SimNeedPin), [&](sys::Message *request) -> sys::MessagePointer {
            auto data = static_cast<cellular::msg::notification::SimNeedPin *>(request);
            if (phoneLockHandler.isPhoneLocked()) {
                simLockHandler.setSimUnlockBlockOnLockedPhone();
            }
            return simLockHandler.handleSimPinRequest(data->attempts);
        });
        connect(typeid(cellular::msg::request::sim::PinUnlock::Response),
                [&](sys::Message *request) -> sys::MessagePointer {
                    auto data = static_cast<cellular::msg::request::sim::PinUnlock::Response *>(request);
                    if (data->retCode) {
                        return simLockHandler.handleSimUnlockedMessage();
                    }
                    return sys::msgNotHandled();
                });
        connect(typeid(locks::UnLockSimInput), [&](sys::Message *request) -> sys::MessagePointer {
            auto msg = static_cast<locks::UnLockSimInput *>(request);
            return simLockHandler.verifySimLockInput(msg->getInputData());
        });
        connect(typeid(locks::ResetSimLockState), [&](sys::Message *request) -> sys::MessagePointer {
            return simLockHandler.handleResetSimLockStateRequest();
        });
        connect(typeid(cellular::msg::notification::SimNeedPuk), [&](sys::Message *request) -> sys::MessagePointer {
            auto data = static_cast<cellular::msg::notification::SimNeedPuk *>(request);
            if (phoneLockHandler.isPhoneLocked()) {
                simLockHandler.setSimUnlockBlockOnLockedPhone();
            }
            return simLockHandler.handleSimPukRequest(data->attempts);
        });
        connect(typeid(cellular::msg::request::sim::UnblockWithPuk::Response),
                [&](sys::Message *request) -> sys::MessagePointer {
                    auto data = static_cast<cellular::msg::request::sim::UnblockWithPuk::Response *>(request);
                    if (data->retCode) {
                        return simLockHandler.handleSimUnlockedMessage();
                    }
                    return sys::msgNotHandled();
                });
        connect(typeid(locks::ChangeSimPin), [&](sys::Message *request) -> sys::MessagePointer {
            return simLockHandler.handleSimPinChangeRequest();
        });
        connect(typeid(cellular::msg::request::sim::ChangePin::Response),
                [&](sys::Message *request) -> sys::MessagePointer {
                    auto data = static_cast<cellular::msg::request::sim::ChangePin::Response *>(request);
                    if (data->retCode) {
                        return simLockHandler.handleSimPinChangedMessage();
                    }
                    else {
                        return simLockHandler.handleSimPinChangeFailedRequest();
                    }
                });
        connect(typeid(locks::EnableSimPin),
                [&](sys::Message *request) -> sys::MessagePointer { return simLockHandler.handleSimEnableRequest(); });
        connect(typeid(locks::DisableSimPin),
                [&](sys::Message *request) -> sys::MessagePointer { return simLockHandler.handleSimDisableRequest(); });
        connect(typeid(cellular::msg::request::sim::SetPinLock::Response),
                [&](sys::Message *request) -> sys::MessagePointer {
                    auto data = static_cast<cellular::msg::request::sim::SetPinLock::Response *>(request);
                    if (data->retCode) {
                        return simLockHandler.handleSimAvailabilityMessage();
                    }
                    else {
                        if (data->lock == cellular::api::SimLockState::Enabled) {
                            return simLockHandler.handleSimEnableRequest();
                        }
                        else {
                            return simLockHandler.handleSimDisableRequest();
                        }
                    }
                });
        connect(typeid(cellular::msg::notification::SimBlocked), [&](sys::Message *request) -> sys::MessagePointer {
            if (phoneLockHandler.isPhoneLocked()) {
                simLockHandler.setSimUnlockBlockOnLockedPhone();
            }
            return simLockHandler.handleSimBlockedRequest();
        });
        connect(typeid(cellular::msg::notification::UnhandledCME), [&](sys::Message *request) -> sys::MessagePointer {
            auto data = static_cast<cellular::msg::notification::UnhandledCME *>(request);
            if (phoneLockHandler.isPhoneLocked()) {
                simLockHandler.setSimUnlockBlockOnLockedPhone();
            }
            return simLockHandler.handleCMEErrorRequest(data->code);
        });
        connect(typeid(cellular::msg::notification::SimNotInserted), [&](sys::Message *request) -> sys::MessagePointer {
            return simLockHandler.handleSimNotInsertedMessage();
        });
        connect(typeid(locks::SetSim), [&](sys::Message *request) -> sys::MessagePointer {
            auto data = static_cast<locks::SetSim *>(request);
            simLockHandler.setSim(data->getSimSlot());
            return sys::msgHandled();
        });
        connect(typeid(cellular::msg::request::sim::SetActiveSim::Response),
                [&](sys::Message *request) -> sys::MessagePointer {
                    auto data = static_cast<cellular::msg::request::sim::SetActiveSim::Response *>(request);
                    if (data->retCode) {
                        settings->setValue(::settings::SystemProperties::activeSim,
                                           utils::enumToString(Store::GSM::get()->selected),
                                           ::settings::SettingsScope::Global);
                        return sys::msgHandled();
                    }
                    return sys::msgNotHandled();
                });
        connect(typeid(cellular::msg::notification::SimReady),
                [&](sys::Message *request) -> sys::MessagePointer { return simLockHandler.handleSimReadyMessage(); });
        connect(typeid(cellular::msg::notification::ModemStateChanged),
                [&](sys::Message *request) -> sys::MessagePointer {
                    auto data = static_cast<cellular::msg::notification::ModemStateChanged *>(request);
                    if (data->state == cellular::api::ModemState::Ready) {
                        simLockHandler.setSimReady();
                        simLockHandler.getSettingsSimSelect(
                            settings->getValue(settings::SystemProperties::activeSim, settings::SettingsScope::Global));
                        return sys::msgHandled();
                    }
                    return sys::msgNotHandled();
                });

        connect(typeid(sys::bluetooth::BluetoothModeChanged), [&](sys::Message *request) -> sys::MessagePointer {
            auto data = static_cast<sys::bluetooth::BluetoothModeChanged *>(request);
            handleBluetoothModeChanged(data->getBluetoothMode());
            return sys::msgHandled();
        });
        connect(typeid(message::bluetooth::RequestAuthenticate), [&](sys::Message *msg) {
            auto m = dynamic_cast<::message::bluetooth::RequestAuthenticate *>(msg);

            auto data = std::make_unique<gui::BluetoothAuthenticateRequestParams>(
                m->getDevice(), m->getAuthenticateType(), m->getPairingCode());
            data->setDisposition(gui::popup::Disposition{gui::popup::Disposition::Priority::High,
                                                         gui::popup::Disposition::WindowType::Popup,
                                                         gui::popup::ID::BluetoothAuthenticate});
            actionsRegistry.enqueue(ActionEntry{actions::ShowPopup, std::move(data)});

            return sys::MessageNone{};
        });
        connect(typeid(BluetoothPairResultMessage), [&](sys::Message *msg) {
            auto m = dynamic_cast<BluetoothPairResultMessage *>(msg);

            auto data = std::make_unique<gui::BluetoothInfoParams>(m->getDevice(), m->isSucceed(), m->getErrorCode());
            data->setDisposition(gui::popup::Disposition{gui::popup::Disposition::Priority::High,
                                                         gui::popup::Disposition::WindowType::Popup,
                                                         gui::popup::ID::BluetoothInfo});
            actionsRegistry.enqueue(ActionEntry{actions::ShowPopup, std::move(data)});

            return sys::MessageNone{};
        });

        alarms::AlarmServiceAPI::requestRegisterSnoozedAlarmsCountChangeCallback(this);
        connect(typeid(alarms::SnoozedAlarmsCountChangeMessage), [&](sys::Message *request) -> sys::MessagePointer {
            auto data = static_cast<alarms::SnoozedAlarmsCountChangeMessage *>(request);
            handleSnoozeCountChange(data->getSnoozedCount());
            return sys::msgHandled();
        });

        alarms::AlarmServiceAPI::requestRegisterActiveAlarmsIndicatorHandler(this);
        connect(typeid(alarms::ActiveAlarmMessage), [&](sys::Message *request) -> sys::MessagePointer {
            auto data = static_cast<alarms::ActiveAlarmMessage *>(request);
            handleAlarmClockStatusChanged(data->isAnyAlarmActive());
            return sys::msgHandled();
        });

        auto convertibleToActionHandler = [this](sys::Message *request) { return handleMessageAsAction(request); };
        connect(typeid(cellular::MMIResultMessage), convertibleToActionHandler);
        connect(typeid(cellular::MMIResponseMessage), convertibleToActionHandler);
        connect(typeid(cellular::MMIPushMessage), convertibleToActionHandler);
        connect(typeid(cellular::NoSimNotification), convertibleToActionHandler);
        connect(typeid(cellular::NotAnEmergencyNotification), convertibleToActionHandler);
        connect(typeid(cellular::NoNetworkConenctionNotification), convertibleToActionHandler);
        connect(typeid(cellular::CallRequestGeneralError), convertibleToActionHandler);
        connect(typeid(cellular::SmsNoSimRequestMessage), convertibleToActionHandler);
        connect(typeid(cellular::SMSRejectedByOfflineNotification), convertibleToActionHandler);
        connect(typeid(cellular::CallRejectedByOfflineNotification), convertibleToActionHandler);
        connect(typeid(sys::TetheringQuestionRequest), convertibleToActionHandler);
        connect(typeid(sys::TetheringQuestionAbort), convertibleToActionHandler);
        connect(typeid(sys::TetheringPhoneModeChangeProhibitedMessage), convertibleToActionHandler);
        connect(typeid(cellular::CallAbortedNotification), convertibleToActionHandler);
        connect(typeid(cellular::RingingMessage), convertibleToActionHandler);
        connect(typeid(cellular::CallActiveNotification), convertibleToActionHandler);
        connect(typeid(cellular::HangupCallMessage), convertibleToActionHandler);
    }

    void ApplicationManager::handlePhoneModeChanged(sys::phone_modes::PhoneMode phoneMode)
    {
        for (const auto app : getStackedApplications()) {
            changePhoneMode(phoneMode, app);
        }
    }

    void ApplicationManager::changePhoneMode(sys::phone_modes::PhoneMode phoneMode, const ApplicationHandle *app)
    {
        ActionEntry action{actions::PhoneModeChanged, std::make_unique<gui::PhoneModeParams>(phoneMode)};
        action.setTargetApplication(app->name());
        actionsRegistry.enqueue(std::move(action));
    }

    void ApplicationManager::handleBluetoothModeChanged(sys::bluetooth::BluetoothMode mode)
    {
        bluetoothMode = mode;
        for (const auto app : getStackedApplications()) {
            changeBluetoothMode(app);
        }
    }

    void ApplicationManager::changeBluetoothMode(const ApplicationHandle *app)
    {
        ActionEntry action{actions::BluetoothModeChanged, std::make_unique<gui::BluetoothModeParams>(bluetoothMode)};
        action.setTargetApplication(app->name());
        actionsRegistry.enqueue(std::move(action));
    }

    void ApplicationManager::handleAlarmClockStatusChanged(bool status)
    {
        if (alarmClockStatus != status) {
            alarmClockStatus = status;
            for (const auto app : getStackedApplications()) {
                changeAlarmClockStatus(app);
            }
        }
    }

    void ApplicationManager::changeAlarmClockStatus(const ApplicationHandle *app)
    {
        ActionEntry action{actions::AlarmClockStatusChanged,
                           std::make_unique<app::AlarmClockStatusParams>(alarmClockStatus)};
        action.setTargetApplication(app->name());
        actionsRegistry.enqueue(std::move(action));
    }

    void ApplicationManager::handleTetheringChanged(sys::phone_modes::Tethering tethering)
    {
        notificationProvider.handle(tethering);
    }

    void ApplicationManager::handleSnoozeCountChange(unsigned snoozeCount)
    {
        notificationProvider.handleSnooze(snoozeCount);
    }

    auto ApplicationManager::handleAutoLockSetRequest(SetAutoLockTimeoutRequest *request)
        -> std::shared_ptr<sys::ResponseMessage>
    {
        auto interval = request->getValue();
        settings->setValue(settings::SystemProperties::autoLockTimeInSec,
                           utils::to_string(interval.count()),
                           settings::SettingsScope::Global);
        autoLockTimer.restart(interval);
        return std::make_shared<sys::ResponseMessage>();
    }

    auto ApplicationManager::handleDeveloperModeRequest(sys::Message *request) -> sys::MessagePointer
    {
        if (auto msg = dynamic_cast<sdesktop::developerMode::DeveloperModeRequest *>(request)) {
            if (dynamic_cast<sdesktop::developerMode::ScreenlockCheckEvent *>(msg->event.get())) {
                auto response = std::make_shared<sdesktop::developerMode::DeveloperModeRequest>(
                    std::make_unique<sdesktop::developerMode::ScreenlockCheckEvent>(phoneLockHandler.isPhoneLocked()));
                bus.sendUnicast(std::move(response), service::name::service_desktop);
                return sys::msgHandled();
            }
        }
        return sys::msgNotHandled();
    }

    void ApplicationManager::lockTimeChanged(std::string value)
    {
        if (value.empty()) {
            LOG_ERROR("No value for auto-locking time period, request ignored");
            return;
        }
        const auto interval = std::chrono::seconds{utils::getNumericValue<unsigned int>(value)};
        if (interval.count() == 0) {
            LOG_ERROR("Invalid auto-locking time period of 0s, request ignored");
            return;
        }
        autoLockTimer.restart(interval);
    }

    void ApplicationManager::onPhoneLocked()
    {
        if (phoneLockHandler.isPhoneLocked()) {
            autoLockTimer.stop();
            return;
        }
        if (auto focusedApp = getFocusedApplication(); focusedApp == nullptr || focusedApp->preventsAutoLocking()) {
            autoLockTimer.start();
            return;
        }
        if (phoneModeObserver->isTetheringOn()) {
            autoLockTimer.start();
            return;
        }
        if (event::service::api::isTorchOn()) {
            autoLockTimer.start();
            return;
        }
        phoneLockHandler.handleLockRequest();
    }

    auto ApplicationManager::startApplication(ApplicationHandle &app) -> bool
    {
        if (not ApplicationManagerCommon::startApplication(app)) {
            if (ApplicationManagerCommon::isApplicationStarting(app)) {
                LOG_INFO("%s is starting already...", app.name().c_str());
                return false;
            }

            LOG_INFO("Starting application %s", app.name().c_str());
            StatusIndicators statusIndicators;
            statusIndicators.phoneMode        = phoneModeObserver->getCurrentPhoneMode();
            statusIndicators.bluetoothMode    = bluetoothMode;
            statusIndicators.alarmClockStatus = alarmClockStatus;
            app.run(statusIndicators, this);
        }
        return true;
    }

    auto ApplicationManager::resolveHomeWindow() -> std::string
    {
        return phoneLockHandler.isPhoneLocked() ? gui::popup::window::phone_lock_window
                                                : gui::name::window::main_window;
    }

    auto ApplicationManager::resolveHomeApplication() -> std::string
    {
        if (checkOnBoarding()) {
            phoneLockHandler.handleUnlockRequest();
            return app::name_onboarding;
        }
        return rootApplicationName;
    }

    auto ApplicationManager::handleDBResponse(db::QueryResponse *msg) -> bool
    {
        auto result = msg->getResult();
        if (auto response = dynamic_cast<db::query::notifications::GetAllResult *>(result.get())) {
            notificationProvider.handle(response);
            return true;
        }
        return false;
    }

    auto ApplicationManager::handlePhoneModeChangedAction(ActionEntry &action) -> ActionProcessStatus
    {
        const auto &targetName = action.target;
        auto targetApp         = getApplication(targetName);
        if (targetApp == nullptr || !targetApp->handles(action.actionId)) {
            return ActionProcessStatus::Dropped;
        }

        if (targetApp->state() == ApplicationHandle::State::ACTIVE_FORGROUND ||
            targetApp->state() == ApplicationHandle::State::ACTIVE_BACKGROUND) {
            app::ApplicationCommon::requestAction(this, targetName, action.actionId, std::move(action.params));
            return ActionProcessStatus::Accepted;
        }
        return ActionProcessStatus::Skipped;
    }

    auto ApplicationManager::handleAction(ActionEntry &action) -> ActionProcessStatus
    {
        switch (action.actionId) {
        case actions::BluetoothModeChanged:
        case actions::PhoneModeChanged:
        case actions::AlarmClockStatusChanged:
            return handleActionOnActiveApps(action);
        default:
            return ApplicationManagerCommon::handleAction(action);
        }
    }

    void ApplicationManager::handleStart(StartAllowedMessage *msg)
    {
        switch (msg->getStartupType()) {
        case StartupType::Regular:
            ApplicationManagerCommon::handleStart(msg);
            break;
        case StartupType::LowBattery:
        case StartupType::CriticalBattery:
            handleSwitchApplication(std::make_unique<SwitchRequest>(
                                        service::name::appmgr, app::name_desktop, window::name::dead_battery, nullptr)
                                        .get());
            break;
        case StartupType::LowBatteryCharging:
            handleSwitchApplication(
                std::make_unique<SwitchRequest>(
                    service::name::appmgr, app::name_desktop, window::name::charging_battery, nullptr)
                    .get());
            break;
        }
    }

    auto ApplicationManager::handleDisplayLanguageChange(app::manager::DisplayLanguageChangeRequest *msg) -> bool
    {
        const auto &requestedLanguage = msg->getLanguage();

        if (not utils::setDisplayLanguage(requestedLanguage)) {
            LOG_WARN("The selected language is already set. Ignore.");
            return false;
        }
        settings->setValue(
            settings::SystemProperties::displayLanguage, requestedLanguage, settings::SettingsScope::Global);
        rebuildActiveApplications();
        DBServiceAPI::InformLanguageChanged(this);
        return true;
    }
} // namespace app::manager
