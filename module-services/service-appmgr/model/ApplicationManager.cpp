// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-appmgr/model/ApplicationManager.hpp>
#include <service-appmgr/Controller.hpp>

#include <apps-common/messages/AppMessage.hpp>
#include <apps-common/popups/data/PhoneModeParams.hpp>
#include <apps-common/popups/data/PopupRequestParams.hpp>
#include <Common.hpp>
#include <Service/Message.hpp>
#include <module-sys/Timers/TimerFactory.hpp>
#include <SystemManager/SystemManager.hpp>
#include <SystemManager/messages/SystemManagerMessage.hpp>
#include <SystemManager/messages/TetheringQuestionRequest.hpp>
#include <SystemManager/messages/TetheringPhoneModeChangeProhibitedMessage.hpp>
#include <application-call/ApplicationCall.hpp>
#include <application-special-input/ApplicationSpecialInput.hpp>
#include <application-desktop/ApplicationDesktop.hpp>
#include <application-onboarding/ApplicationOnBoarding.hpp>
#include <application-onboarding/data/OnBoardingMessages.hpp>
#include <i18n/i18n.hpp>
#include <log/log.hpp>
#include <service-appmgr/messages/Message.hpp>
#include <service-evtmgr/EventManager.hpp>
#include <service-evtmgr/EVMessages.hpp>
#include <service-gui/ServiceGUI.hpp>
#include <service-eink/ServiceEink.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <service-desktop/Constants.hpp>
#include <service-appmgr/StartupType.hpp>
#include <module-services/service-audio/service-audio/AudioMessage.hpp>

#include <algorithm>
#include <limits>
#include <utility>
#include <module-utils/Utils.hpp>
#include <module-services/service-db/agents/settings/SystemSettings.hpp>
#include <service-appmgr/messages/DOMRequest.hpp>
#include <service-appmgr/messages/GetAllNotificationsRequest.hpp>
#include <service-appmgr/messages/AutoLockRequests.hpp>
#include <service-appmgr/messages/FinishRequest.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <module-db/queries/notifications/QueryNotificationsGetAll.hpp>
#include <service-cellular-api>

#include "module-services/service-appmgr/service-appmgr/messages/ApplicationStatus.hpp"
#include <event-manager-api>

namespace app::manager
{
    namespace
    {
        constexpr auto ApplicationManagerStackDepth = 3072;
        constexpr auto autoLockTimerName            = "AutoLockTimer";
    } // namespace

    ApplicationManagerBase::ApplicationManagerBase(std::vector<std::unique_ptr<app::ApplicationLauncher>> &&launchers)
        : applications{std::move(launchers)}
    {}

    void ApplicationManagerBase::setState(State _state) noexcept
    {
        state = _state;
    }

    auto ApplicationManagerBase::getFocusedApplication() const noexcept -> ApplicationHandle *
    {
        for (const auto &item : stack) {
            if (auto app = getApplication(item.appName);
                app != nullptr && app->state() == app::Application::State::ACTIVE_FORGROUND) {
                return app;
            }
        }
        return nullptr;
    }

    auto ApplicationManagerBase::getLaunchingApplication() const noexcept -> ApplicationHandle *
    {
        if (stack.isEmpty()) {
            return nullptr;
        }
        auto app = getApplication(stack.front().appName);
        return app->state() != app::Application::State::ACTIVE_FORGROUND ? app : nullptr;
    }

    auto ApplicationManagerBase::getPreviousApplication() const noexcept -> ApplicationHandle *
    {
        if (stack.size() < 2) {
            return nullptr;
        }
        return getApplication(stack[1].appName);
    }

    auto ApplicationManagerBase::getApplication(const ApplicationName &name) const noexcept -> ApplicationHandle *
    {
        return applications.findByName(name);
    }

    auto ApplicationManagerBase::isApplicationCloseable(const ApplicationHandle *app) const noexcept -> bool
    {
        if (stack.contains(app->name())) {
            const auto &appOccurrences = stack.find(app->name());
            return app->closeable() && std::none_of(appOccurrences.begin(), appOccurrences.end(), [](const auto &item) {
                       return !item.isCloseable;
                   });
        }
        return app->closeable();
    }

    auto ApplicationManagerBase::getStackedApplications() noexcept -> std::vector<ApplicationHandle *>
    {
        const auto &uniqueApps = stack.unique();
        std::vector<ApplicationHandle *> runningApps;
        std::transform(uniqueApps.begin(),
                       uniqueApps.end(),
                       std::back_inserter(runningApps),
                       [this](const auto &appName) { return getApplication(appName); });
        return runningApps;
    }

    ApplicationManager::ApplicationManager(const ApplicationName &serviceName,
                                           std::vector<std::unique_ptr<app::ApplicationLauncher>> &&launchers,
                                           const ApplicationName &_rootApplicationName)
        : Service{serviceName, {}, ApplicationManagerStackDepth},
          ApplicationManagerBase(std::move(launchers)), rootApplicationName{_rootApplicationName},
          actionsRegistry{[this](ActionEntry &action) { return handleAction(action); }}, notificationProvider(this),
          settings(std::make_unique<settings::Settings>()),
          phoneModeObserver(std::make_unique<sys::phone_modes::Observer>()),
          phoneLockHandler(locks::PhoneLockHandler(this, settings)), simLockHandler(this)
    {
        autoLockTimer = sys::TimerFactory::createSingleShotTimer(
            this, autoLockTimerName, sys::timer::InfiniteTimeout, [this](sys::Timer &) { onPhoneLocked(); });
        bus.channels.push_back(sys::BusChannel::PhoneModeChanges);
        bus.channels.push_back(sys::BusChannel::ServiceAudioNotifications);
        bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
        bus.channels.push_back(sys::BusChannel::PhoneLockChanges);
        bus.channels.push_back(sys::BusChannel::ServiceCellularNotifications);
        registerMessageHandlers();
    }

    sys::ReturnCodes ApplicationManager::InitHandler()
    {
        settings->init(service::ServiceProxy(shared_from_this()));
        utils::setDisplayLanguage(
            settings->getValue(settings::SystemProperties::displayLanguage, settings::SettingsScope::Global));

        phoneLockHandler.enablePhoneLock((utils::getNumericValue<bool>(
            settings->getValue(settings::SystemProperties::lockScreenPasscodeIsOn, settings::SettingsScope::Global))));

        phoneLockHandler.setPhoneLockHash(
            settings->getValue(settings::SystemProperties::lockPassHash, settings::SettingsScope::Global));

        settings->registerValueChange(
            settings::SystemProperties::lockScreenPasscodeIsOn,
            [this](const std::string &value) { phoneLockHandler.enablePhoneLock(utils::getNumericValue<bool>(value)); },
            settings::SettingsScope::Global);

        settings->registerValueChange(
            settings::SystemProperties::lockPassHash,
            [this](const std::string &value) { phoneLockHandler.setPhoneLockHash(value); },
            settings::SettingsScope::Global);

        settings->registerValueChange(
            settings::SystemProperties::displayLanguage,
            [this](std::string value) { displayLanguageChanged(std::move(value)); },
            settings::SettingsScope::Global);
        settings->registerValueChange(
            settings::SystemProperties::inputLanguage,
            [this](std::string value) { inputLanguageChanged(std::move(value)); },
            settings::SettingsScope::Global);
        settings->registerValueChange(
            settings::SystemProperties::autoLockTimeInSec,
            [this](std::string value) { lockTimeChanged(std::move(value)); },
            settings::SettingsScope::Global);
        settings->registerValueChange(
            ::settings::KeypadLight::state,
            [this](const std::string &value) {
                const auto keypadLightState =
                    static_cast<bsp::keypad_backlight::State>(utils::getNumericValue<int>(value));
                processKeypadBacklightState(keypadLightState);
            },
            ::settings::SettingsScope::Global);

        startBackgroundApplications();
        bus.sendUnicast(std::make_unique<CheckIfStartAllowedMessage>(), service::name::system_manager);

        return sys::ReturnCodes::Success;
    }

    void ApplicationManager::handleStart(StartAllowedMessage *msg)
    {
        switch (msg->getStartupType()) {
        case StartupType::Regular:
            if (auto app = getApplication(rootApplicationName); app != nullptr) {
                Controller::sendAction(this, actions::Home);
            }
            break;
        case StartupType::LowBattery:
            handleSwitchApplication(
                std::make_unique<SwitchRequest>(ServiceName, app::name_desktop, window::name::dead_battery, nullptr)
                    .get());
            break;
        case StartupType::LowBatteryCharging:
            handleSwitchApplication(
                std::make_unique<SwitchRequest>(ServiceName, app::name_desktop, window::name::charging_battery, nullptr)
                    .get());
            break;
        }
    }

    void ApplicationManager::suspendSystemServices()
    {
        sys::SystemManager::SuspendService(service::name::gui, this);
        sys::SystemManager::SuspendService(service::name::eink, this);
    }

    void ApplicationManager::startBackgroundApplications()
    {
        for (const auto &name : std::vector<ApplicationName>{app::special_input}) {
            if (auto app = getApplication(name); app != nullptr) {
                app->runInBackground(phoneModeObserver->getCurrentPhoneMode(), this);
            }
        }
    }

    sys::ReturnCodes ApplicationManager::DeinitHandler()
    {
        settings->deinit();
        closeApplications();
        return sys::ReturnCodes::Success;
    }

    auto ApplicationManager::ProcessCloseReason(sys::CloseReason closeReason) -> void
    {
        ActionRequest act = ActionRequest{this->GetName(), app::manager::actions::DisplayLogoAtExit, nullptr};
        switch (closeReason) {
        case sys::CloseReason::SystemBrownout:
            [[fallthrough]];
        case sys::CloseReason::LowBattery:
            act = ActionRequest{this->GetName(), app::manager::actions::SystemBrownout, nullptr};
            break;
        case sys::CloseReason::RegularPowerDown:
            [[fallthrough]];
        case sys::CloseReason::Reboot:
            break;
        }
        handleActionRequest(&act);
        sendCloseReadyMessage(this);
    }

    auto ApplicationManager::DataReceivedHandler([[maybe_unused]] sys::DataMessage *msgl,
                                                 [[maybe_unused]] sys::ResponseMessage *resp) -> sys::MessagePointer
    {
        return std::make_shared<sys::ResponseMessage>();
    }

    void ApplicationManager::registerMessageHandlers()
    {
        phoneModeObserver->connect(this);
        phoneModeObserver->subscribe([this](sys::phone_modes::PhoneMode phoneMode) {
            handlePhoneModeChanged(phoneMode);
            actionsRegistry.enqueue(
                ActionEntry{actions::ShowPopup, std::make_unique<gui::PhoneModePopupRequestParams>(phoneMode)});
        });

        phoneModeObserver->subscribe(
            [this](sys::phone_modes::Tethering tethering) { handleTetheringChanged(tethering); });

        connect(typeid(StartAllowedMessage), [this](sys::Message *request) {
            auto msg = static_cast<StartAllowedMessage *>(request);
            handleStart(msg);
            return sys::MessageNone{};
        });
        connect(typeid(ApplicationStatusRequest), [this](sys::Message *request) {
            auto msg = static_cast<ApplicationStatusRequest *>(request);
            return std::make_shared<ApplicationStatusResponse>(msg->checkAppName,
                                                               getApplication(msg->checkAppName) != nullptr);
        });
        connect(typeid(PowerSaveModeInitRequest), [this]([[maybe_unused]] sys::Message *msg) {
            handlePowerSavingModeInit();
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(PreventBlockingRequest), [this]([[maybe_unused]] sys::Message *msg) {
            if (!phoneLockHandler.isPhoneLocked()) {
                autoLockTimer.start();
            }
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(SwitchRequest), [this](sys::Message *request) {
            auto msg = static_cast<SwitchRequest *>(request);
            handleSwitchApplication(msg);
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(SwitchBackRequest), [this](sys::Message *request) {
            auto msg = static_cast<SwitchBackRequest *>(request);
            handleSwitchBack(msg);
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(SwitchConfirmation), [this](sys::Message *request) {
            auto msg = static_cast<SwitchConfirmation *>(request);
            handleSwitchConfirmation(msg);
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(CloseConfirmation), [this](sys::Message *request) {
            auto msg = static_cast<CloseConfirmation *>(request);
            handleCloseConfirmation(msg);
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(ApplicationCloseRequest), [this](sys::Message *request) {
            auto msg = static_cast<ApplicationCloseRequest *>(request);
            closeApplication(applications.findByName(msg->getApplication()));
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(ApplicationInitialised), [this](sys::Message *request) {
            auto msg = static_cast<ApplicationInitialised *>(request);
            handleInitApplication(msg);
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(DisplayLanguageChangeRequest), [this](sys::Message *request) {
            auto msg = static_cast<DisplayLanguageChangeRequest *>(request);
            handleDisplayLanguageChange(msg);
            return std::make_shared<GetCurrentDisplayLanguageResponse>(utils::getDisplayLanguage());
        });
        connect(typeid(InputLanguageChangeRequest), [this](sys::Message *request) {
            auto msg = static_cast<InputLanguageChangeRequest *>(request);
            handleInputLanguageChange(msg);
            return sys::msgHandled();
        });
        connect(typeid(ShutdownRequest), [this](sys::Message *) {
            closeApplications();
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(ActionRequest), [this](sys::Message *request) {
            auto actionMsg = static_cast<ActionRequest *>(request);
            handleActionRequest(actionMsg);
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(FinishRequest), [this](sys::Message *request) {
            auto finishMsg = static_cast<FinishRequest *>(request);
            stack.eraseFirstOf(finishMsg->sender);
            closeNoLongerNeededApplications();
            return sys::msgHandled();
        });
        connect(typeid(ActionHandledResponse), [this](sys::Message *response) {
            actionsRegistry.finished();
            return sys::MessageNone{};
        });
        connect(typeid(GetCurrentDisplayLanguageRequest), [&](sys::Message *request) {
            return std::make_shared<GetCurrentDisplayLanguageResponse>(utils::getDisplayLanguage());
        });
        connect(typeid(UpdateInProgress), [this](sys::Message *) {
            closeApplicationsOnUpdate();
            return sys::msgHandled();
        });
        connect(typeid(SetOsUpdateVersion), [this](sys::Message *request) {
            auto msg = static_cast<SetOsUpdateVersion *>(request);
            handleSetOsUpdateVersionChange(msg);
            return sys::msgHandled();
        });
        connect(typeid(GetAllNotificationsRequest), [&](sys::Message *request) {
            notificationProvider.requestNotSeenNotifications();
            notificationProvider.send();
            return sys::msgHandled();
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
        connect(typeid(GetAutoLockTimeoutRequest), [&](sys::Message *request) -> sys::MessagePointer {
            auto req = static_cast<GetAutoLockTimeoutRequest *>(request);
            return handleAutoLockGetRequest(req);
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

        connect(typeid(onBoarding::FinalizeOnBoarding),
                [&](sys::Message *request) -> sys::MessagePointer { return handleOnBoardingFinalize(); });

        connect(typeid(sdesktop::developerMode::DeveloperModeRequest),
                [&](sys::Message *request) -> sys::MessagePointer { return handleDeveloperModeRequest(request); });

        connect(typeid(app::manager::DOMRequest), [&](sys::Message *request) { return handleDOMRequest(request); });

        auto convertibleToActionHandler = [this](sys::Message *request) { return handleMessageAsAction(request); };
        connect(typeid(CellularMMIResultMessage), convertibleToActionHandler);
        connect(typeid(CellularMMIResponseMessage), convertibleToActionHandler);
        connect(typeid(CellularMMIPushMessage), convertibleToActionHandler);
        connect(typeid(CellularNoSimNotification), convertibleToActionHandler);
        connect(typeid(CellularNotAnEmergencyNotification), convertibleToActionHandler);
        connect(typeid(sys::CriticalBatteryLevelNotification), convertibleToActionHandler);
        connect(typeid(CellularSmsNoSimRequestMessage), convertibleToActionHandler);
        connect(typeid(CellularSMSRejectedByOfflineNotification), convertibleToActionHandler);
        connect(typeid(CellularCallRejectedByOfflineNotification), convertibleToActionHandler);
        connect(typeid(sys::TetheringQuestionRequest), convertibleToActionHandler);
        connect(typeid(sys::TetheringQuestionAbort), convertibleToActionHandler);
        connect(typeid(sys::TetheringPhoneModeChangeProhibitedMessage), convertibleToActionHandler);
        connect(typeid(VolumeChanged), convertibleToActionHandler);
        connect(typeid(CellularCallAbortedNotification), convertibleToActionHandler);
        connect(typeid(CellularRingingMessage), convertibleToActionHandler);
        connect(typeid(CellularIncominCallMessage), convertibleToActionHandler);
        connect(typeid(CellularCallerIdMessage), convertibleToActionHandler);
        connect(typeid(CellularCallActiveNotification), convertibleToActionHandler);
        connect(typeid(CellularHangupCallMessage), convertibleToActionHandler);
    }

    sys::ReturnCodes ApplicationManager::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
    {
        LOG_INFO("Power mode: %s", c_str(mode));

        switch (mode) {
        case sys::ServicePowerMode ::Active:
            sys::SystemManager::ResumeService(service::name::eink, this);
            sys::SystemManager::ResumeService(service::name::gui, this);
            break;
        case sys::ServicePowerMode ::SuspendToRAM:
            [[fallthrough]];
        case sys::ServicePowerMode ::SuspendToNVM:
            suspendSystemServices();
            break;
        }
        return sys::ReturnCodes::Success;
    }

    auto ApplicationManager::startApplication(ApplicationHandle &app) -> bool
    {
        if (app.state() == ApplicationHandle::State::ACTIVE_BACKGROUND) {
            LOG_INFO("Switching focus to application [%s] (window [%s])", app.name().c_str(), app.switchWindow.c_str());
            setState(State::AwaitingFocusConfirmation);
            app::Application::messageSwitchApplication(
                this, app.name(), app.switchWindow, std::move(app.switchData), StartupReason::Launch);
        }
        else {
            LOG_INFO("Starting application %s", app.name().c_str());
            app.run(phoneModeObserver->getCurrentPhoneMode(), this);
        }
        return true;
    }

    auto ApplicationManager::closeApplications() -> bool
    {
        for (const auto &app : getApplications()) {
            if (app->started()) {
                LOG_INFO("Closing application %s", app->name().c_str());
                closeApplication(app.get());
                app->setState(ApplicationHandle::State::DEACTIVATED);
            }
        }
        return true;
    }

    void ApplicationManager::closeNoLongerNeededApplications()
    {
        for (const auto &app : getApplications()) {
            if (app->started() && app->closeable() && !stack.contains(app->name())) {
                closeApplication(app.get());
                app->setState(ApplicationHandle::State::DEACTIVATED);
            }
        }
    }

    auto ApplicationManager::closeApplicationsOnUpdate() -> bool
    {
        for (const auto &app : getApplications()) {
            if (app->started()) {
                auto appName = app->name();
                if (appName == app::name_desktop) {
                    LOG_DEBUG("Delay closing %s", app::name_desktop);
                    continue;
                }
                LOG_INFO("Closing application on Update %s", appName.c_str());
                closeApplication(app.get());
                app->setState(ApplicationHandle::State::DEACTIVATED);
            }
        }
        return true;
    }

    void ApplicationManager::closeApplication(ApplicationHandle *application)
    {
        if (application == nullptr) {
            return;
        }

        if (sys::SystemManager::DestroyApplication(application->name(), this)) {
            LOG_INFO("Application %s closed", application->name().c_str());
        }
        else {
            LOG_FATAL("Application %s is still running", application->name().c_str());
        }
        application->close();
    }

    auto ApplicationManager::handlePowerSavingModeInit() -> bool
    {
        LOG_INFO("Going to suspend mode");
        return true;
    }

    auto ApplicationManager::handleSwitchApplication(SwitchRequest *msg, bool closeCurrentlyFocusedApp) -> bool
    {
        auto app = getApplication(msg->getName());
        if (app == nullptr) {
            LOG_ERROR("Failed to switch to application %s: No such application.", msg->getName().c_str());
            return false;
        }

        auto currentlyFocusedApp = getFocusedApplication();
        if (currentlyFocusedApp == nullptr) {
            LOG_INFO("No focused application at the moment. Starting new application...");
            onApplicationSwitch(*app, std::move(msg->getData()), msg->getWindow());
            startApplication(*app);
            return false;
        }

        LOG_DEBUG("Switch applications: [%s][%s](%s) -> [%s][%s](%s)",
                  currentlyFocusedApp->name().c_str(),
                  currentlyFocusedApp->switchWindow.c_str(),
                  app::Application::stateStr(currentlyFocusedApp->state()),
                  app->name().c_str(),
                  app->switchWindow.c_str(),
                  app::Application::stateStr(app->state()));

        stack.front().isCloseable = closeCurrentlyFocusedApp;
        onApplicationSwitch(*app, std::move(msg->getData()), msg->getWindow());
        if (app->name() == currentlyFocusedApp->name()) {
            // Switch window only.
            app::Application::messageSwitchApplication(
                this, app->name(), app->switchWindow, std::move(app->switchData), StartupReason::Launch);
            return false;
        }

        requestApplicationClose(*currentlyFocusedApp, isApplicationCloseable(currentlyFocusedApp));
        return true;
    }

    void ApplicationManager::onApplicationSwitch(ApplicationHandle &nextApp,
                                                 std::unique_ptr<gui::SwitchData> &&data,
                                                 std::string targetWindow)
    {
        if (nextApp.name() == rootApplicationName) {
            stack.clear();
        }

        stack.push({nextApp.name(), true});
        nextApp.switchData   = std::move(data);
        nextApp.switchWindow = std::move(targetWindow);
    }

    void ApplicationManager::requestApplicationClose(ApplicationHandle &app, bool isCloseable)
    {
        if (isCloseable) {
            LOG_INFO("Closing application %s", app.name().c_str());
            setState(State::AwaitingCloseConfirmation);
            app::Application::messageCloseApplication(this, app.name());
        }
        else {
            LOG_INFO("Application %s is about to lose focus.", app.name().c_str());
            setState(State::AwaitingLostFocusConfirmation);
            app::Application::messageApplicationLostFocus(this, app.name());
        }
    }

    void ApplicationManager::handleActionRequest(ActionRequest *actionMsg)
    {
        ActionEntry entry{actionMsg->getAction(), std::move(actionMsg->getData())};
        actionsRegistry.enqueue(std::move(entry));
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

    void ApplicationManager::handleTetheringChanged(sys::phone_modes::Tethering tethering)
    {
        notificationProvider.handle(tethering);
    }

    ActionProcessStatus ApplicationManager::handleAction(ActionEntry &action)
    {
        switch (action.actionId) {
        case actions::Home:
            return handleHomeAction(action);
        case actions::Launch:
            return handleLaunchAction(action);
        case actions::PhoneModeChanged:
            return handlePhoneModeChangedAction(action);
        case actions::ShowPopup:
            [[fallthrough]];
        case actions::AbortPopup:
            return handleActionOnFocusedApp(action);
        case actions::NotificationsChanged:
            return handleActionOnFocusedApp(action);
        default:
            return handleCustomAction(action);
        }
    }

    auto ApplicationManager::handleHomeAction(ActionEntry &action) -> ActionProcessStatus
    {
        action.setTargetApplication(resolveHomeApplication());

        SwitchRequest switchRequest(ServiceName, resolveHomeApplication(), resolveHomeWindow(), nullptr);
        return handleSwitchApplication(&switchRequest) ? ActionProcessStatus::Accepted : ActionProcessStatus::Dropped;
    }

    auto ApplicationManager::resolveHomeWindow() -> std::string
    {
        return phoneLockHandler.isPhoneLocked() ? gui::popup::window::phone_lock_window
                                                : gui::name::window::main_window;
    }

    auto ApplicationManager::handleOnBoardingFinalize() -> sys::MessagePointer
    {
        settings->setValue(settings::SystemProperties::onboardingDone, utils::to_string(true));
        app::manager::Controller::sendAction(this, app::manager::actions::Home);
        return sys::msgHandled();
    }

    auto ApplicationManager::checkOnBoarding() -> bool
    {
        return not utils::getNumericValue<bool>(settings->getValue(settings::SystemProperties::onboardingDone));
    }

    auto ApplicationManager::resolveHomeApplication() -> std::string
    {
        if (checkOnBoarding()) {
            phoneLockHandler.handleUnlockRequest();
            return app::name_onboarding;
        }
        return rootApplicationName;
    }

    auto ApplicationManager::handleLaunchAction(ActionEntry &action) -> ActionProcessStatus
    {
        auto launchParams = static_cast<ApplicationLaunchData *>(action.params.get());
        auto targetApp    = getApplication(launchParams->getTargetApplicationName());
        if (targetApp == nullptr || !targetApp->handles(actions::Launch)) {
            return ActionProcessStatus::Dropped;
        }

        targetApp->startupReason = StartupReason::Launch;
        action.setTargetApplication(targetApp->name());
        SwitchRequest switchRequest(ServiceName, targetApp->name(), gui::name::window::main_window, nullptr);
        return handleSwitchApplication(&switchRequest) ? ActionProcessStatus::Accepted : ActionProcessStatus::Dropped;
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
            app::Application::requestAction(this, targetName, action.actionId, std::move(action.params));
            return ActionProcessStatus::Accepted;
        }
        return ActionProcessStatus::Skipped;
    }

    auto ApplicationManager::handleActionOnFocusedApp(ActionEntry &action) -> ActionProcessStatus
    {
        auto targetApp = getFocusedApplication();
        if (targetApp == nullptr) {
            return ActionProcessStatus::Skipped;
        }
        action.setTargetApplication(targetApp->name());
        auto &params = action.params;
        app::Application::requestAction(this, targetApp->name(), action.actionId, std::move(params));
        return ActionProcessStatus::Accepted;
    }

    auto ApplicationManager::handleCustomAction(ActionEntry &action) -> ActionProcessStatus
    {
        const auto actionHandlers = applications.findByAction(action.actionId);
        if (actionHandlers.empty()) {
            LOG_ERROR("No applications handling action #%d.", action.actionId);
            return ActionProcessStatus::Dropped;
        }
        if (actionHandlers.size() > 1) {
            LOG_FATAL("Choosing amongst multiple action handler applications is not yet implemented.");
            return ActionProcessStatus::Dropped;
        }

        const auto targetApp = actionHandlers.front();
        action.setTargetApplication(targetApp->name());
        auto &actionParams = action.params;
        if (const auto state = targetApp->state(); state == ApplicationHandle::State::ACTIVE_FORGROUND) {
            app::Application::requestAction(this, targetApp->name(), action.actionId, std::move(actionParams));
            return ActionProcessStatus::Accepted;
        }
        else if (state == ApplicationHandle::State::ACTIVE_BACKGROUND) {
            if (const auto result = handleCustomActionOnBackgroundApp(targetApp, action);
                result == ActionProcessStatus::Accepted) {
                return result;
            }
        }

        // Inform that target app switch is caused by Action
        targetApp->startupReason = StartupReason::OnAction;

        const auto focusedAppClose = !(actionParams && actionParams->disableAppClose);
        SwitchRequest switchRequest(
            ServiceName, targetApp->name(), targetApp->switchWindow, std::move(targetApp->switchData));
        return handleSwitchApplication(&switchRequest, focusedAppClose) ? ActionProcessStatus::Accepted
                                                                        : ActionProcessStatus::Skipped;
    }

    auto ApplicationManager::handleCustomActionOnBackgroundApp(ApplicationHandle *app, ActionEntry &action)
        -> ActionProcessStatus
    {
        if (const auto actionFlag = app->actionFlag(action.actionId);
            actionFlag == actions::ActionFlag::AcceptWhenInBackground) {
            app::Application::requestAction(this, app->name(), action.actionId, std::move(action.params));
            return ActionProcessStatus::Accepted;
        }
        return ActionProcessStatus::Dropped;
    }

    auto ApplicationManager::handleSwitchBack(SwitchBackRequest *msg) -> bool
    {
        auto previousApp = getPreviousApplication();
        if (previousApp == nullptr) {
            LOG_WARN("Failed to switch to the previous application: No such application.");
            return false;
        }

        auto currentlyFocusedApp = getFocusedApplication();
        if (currentlyFocusedApp == nullptr) {
            LOG_INFO("No focused application at the moment. Starting previous application...");
            onApplicationSwitchToPrev(*previousApp, std::move(msg->getData()));
            startApplication(*previousApp);
            return true;
        }

        if (previousApp->name() == currentlyFocusedApp->name()) {
            // Switch window only.
            onApplicationSwitchToPrev(*previousApp, std::move(msg->getData()));
            app::Application::messageSwitchBack(this, currentlyFocusedApp->name());
            return true;
        }

        LOG_DEBUG("Switch applications: [%s][%s](%s) -> [%s][%s](%s)",
                  currentlyFocusedApp->name().c_str(),
                  currentlyFocusedApp->switchWindow.c_str(),
                  app::Application::stateStr(currentlyFocusedApp->state()),
                  previousApp->name().c_str(),
                  previousApp->switchWindow.c_str(),
                  app::Application::stateStr(previousApp->state()));

        onApplicationSwitchToPrev(*previousApp, std::move(msg->getData()));
        requestApplicationClose(*currentlyFocusedApp, isApplicationCloseable(currentlyFocusedApp));
        return true;
    }

    void ApplicationManager::onApplicationSwitchToPrev(ApplicationHandle &previousApp,
                                                       std::unique_ptr<gui::SwitchData> &&data)
    {
        stack.pop();
        previousApp.switchData = std::move(data);
    }

    auto ApplicationManager::handleInitApplication(ApplicationInitialised *msg) -> bool
    {
        auto app = getApplication(msg->getSenderName());
        if (app == nullptr) {
            LOG_ERROR("Failed to register %s: No such application.", msg->getSenderName().c_str());
            return false;
        }

        if (msg->getStatus() == StartupStatus::Success) {
            onApplicationInitialised(*app, msg->isBackgroundApplication());
        }
        else {
            onApplicationInitFailure(*app);
        }

        auto notification = std::make_shared<ApplicationStatusRequest>(GetName(), app->name());
        bus.sendMulticast(notification, sys::BusChannel::AppManagerNotifications);
        return true;
    }

    void ApplicationManager::onApplicationInitialised(ApplicationHandle &app, StartInBackground startInBackground)
    {
        LOG_DEBUG("Application %s initialised successfully.", app.name().c_str());

        auto launchingApp = getLaunchingApplication();
        if (launchingApp == nullptr || launchingApp->name() != app.name()) {
            app.setState(ApplicationHandle::State::ACTIVE_BACKGROUND);
            return;
        }

        if (startInBackground) {
            setState(State::Running);
            app.setState(ApplicationHandle::State::ACTIVE_BACKGROUND);
        }
        else {
            LOG_INFO("Switch application to %s", app.name().c_str());
            app.setState(ApplicationHandle::State::ACTIVATING);
            setState(State::AwaitingFocusConfirmation);

            app::Application::messageSwitchApplication(
                this, app.name(), app.switchWindow, std::move(app.switchData), launchingApp->startupReason);
        }
    }

    void ApplicationManager::onApplicationInitFailure(ApplicationHandle &app)
    {
        LOG_ERROR("Failed to initialise %s: Application internal error.", app.name().c_str());
        Controller::switchBack(this);
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
        return true;
    }

    auto ApplicationManager::handleInputLanguageChange(app::manager::InputLanguageChangeRequest *msg) -> bool
    {
        const auto &requestedLanguage = msg->getLanguage();

        if (not utils::setInputLanguage(requestedLanguage)) {
            LOG_WARN("The selected language is already set. Ignore.");
            return false;
        }
        settings->setValue(
            settings::SystemProperties::inputLanguage, requestedLanguage, settings::SettingsScope::Global);
        return true;
    }

    auto ApplicationManager::handleSetOsUpdateVersionChange(SetOsUpdateVersion *msg) -> bool
    {
        LOG_DEBUG("[ApplicationManager::handleSetOsUpdateVersionChange] value ....");
        settings->setValue(
            settings::SystemProperties::osUpdateVersion, msg->osUpdateVer, settings::SettingsScope::Global);

        settings->setValue(
            settings::SystemProperties::osCurrentVersion, msg->osCurrentVer, settings::SettingsScope::Global);
        return true;
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

    void ApplicationManager::rebuildActiveApplications()
    {
        for (const auto &app : getApplications()) {
            if (app && app->valid()) {
                if (const auto appState = app->state(); appState == ApplicationHandle::State::ACTIVE_FORGROUND ||
                                                        appState == ApplicationHandle::State::ACTIVE_BACKGROUND) {
                    app::Application::messageRebuildApplication(this, app->name());
                }
            }
        }
    }

    auto ApplicationManager::handleSwitchConfirmation(SwitchConfirmation *msg) -> bool
    {
        auto senderApp = getApplication(msg->getSenderName());
        if (senderApp == nullptr) {
            LOG_ERROR("Failed to switch to %s. No such application.", msg->getSenderName().c_str());
            return false;
        }
        LOG_INFO(
            "Switch confirmed by %s (%s).", senderApp->name().c_str(), app::Application::stateStr(senderApp->state()));
        return onSwitchConfirmed(*senderApp);
    }

    auto ApplicationManager::onSwitchConfirmed(ApplicationHandle &app) -> bool
    {
        if (getState() == State::AwaitingFocusConfirmation || getState() == State::Running) {
            app.blockClosing = false;
            app.setState(ApplicationHandle::State::ACTIVE_FORGROUND);
            setState(State::Running);
            EventManager::messageSetApplication(this, app.name());
            onLaunchFinished(app);
            return true;
        }
        if (getState() == State::AwaitingLostFocusConfirmation) {
            if (auto launchingApp = getLaunchingApplication(); launchingApp != nullptr) {
                LOG_INFO("Lost focus confirmed by %s. Starting %s application.",
                         app.name().c_str(),
                         launchingApp->name().c_str());
                app.setState(ApplicationHandle::State::ACTIVE_BACKGROUND);
                app.switchWindow.clear();
                startApplication(*launchingApp);
                return true;
            }
        }
        return false;
    }

    void ApplicationManager::onLaunchFinished(ApplicationHandle &app)
    {
        // reset startupReason to default Launch
        app.startupReason = StartupReason::Launch;

        if (!actionsRegistry.hasPendingAction()) {
            return;
        }

        auto action = actionsRegistry.getPendingAction();
        if (app.name() != action->target) {
            return;
        }

        switch (action->actionId) {
        case actions::Home:
            [[fallthrough]];
        case actions::Launch:
            actionsRegistry.finished();
            break;
        default: {
            auto &params = action->params;
            app::Application::requestAction(this, app.name(), action->actionId, std::move(params));
            break;
        }
        }
    }

    auto ApplicationManager::handleCloseConfirmation(CloseConfirmation *msg) -> bool
    {
        auto senderApp = getApplication(msg->getSenderName());
        if (senderApp == nullptr) {
            LOG_ERROR("Failed to handle close confirmation from %s: No such application.",
                      msg->getSenderName().c_str());
            return false;
        }
        return onCloseConfirmed(*senderApp);
    }

    auto ApplicationManager::onCloseConfirmed(ApplicationHandle &app) -> bool
    {
        if (isApplicationCloseable(&app)) {
            app.setState(ApplicationHandle::State::DEACTIVATED);
            Controller::closeApplication(this, app.name());
        }
        else {
            app.setState(ApplicationHandle::State::ACTIVE_BACKGROUND);
        }

        if (const auto launchingApp = getLaunchingApplication();
            launchingApp != nullptr && getState() == State::AwaitingCloseConfirmation) {
            startApplication(*launchingApp);
        }
        return true;
    }

    auto ApplicationManager::handleMessageAsAction(sys::Message *request) -> std::shared_ptr<sys::ResponseMessage>
    {
        auto actionMsg = dynamic_cast<manager::actions::ConvertibleToAction *>(request);
        if (!actionMsg) {
            return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Failure);
        }
        auto action = actionMsg->toAction();
        handleActionRequest(action.get());

        return std::make_shared<sys::ResponseMessage>();
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

    void ApplicationManager::displayLanguageChanged(std::string value)
    {
        if (utils::setDisplayLanguage(value)) {
            rebuildActiveApplications();
        }
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

    auto ApplicationManager::handleAutoLockGetRequest([[maybe_unused]] GetAutoLockTimeoutRequest *request)
        -> std::shared_ptr<sys::ResponseMessage>
    {
        auto intervalValue =
            settings->getValue(settings::SystemProperties::autoLockTimeInSec, settings::SettingsScope::Global);
        const auto interval = std::chrono::seconds{utils::getNumericValue<unsigned int>(intervalValue)};
        return std::make_shared<GetAutoLockTimeoutResponse>(interval);
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

    void ApplicationManager::inputLanguageChanged(std::string value)
    {
        utils::setInputLanguage(value);
    }

    auto ApplicationManager::handleDOMRequest(sys::Message *request) -> std::shared_ptr<sys::ResponseMessage>
    {
        auto app = getFocusedApplication();
        if (app != nullptr) {
            auto message = static_cast<app::manager::DOMRequest *>(request);
            LOG_INFO("DOM request for: %s", message->getSenderName().c_str());
            bus.sendUnicast(std::make_unique<app::manager::DOMRequest>(*message), app->name());
            return sys::MessageNone{};
        }
        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
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

    void ApplicationManager::processKeypadBacklightState(bsp::keypad_backlight::State keypadLightState)
    {
        auto action = bsp::keypad_backlight::Action::turnOff;
        switch (keypadLightState) {
        case bsp::keypad_backlight::State::on:
            action = bsp::keypad_backlight::Action::turnOn;
            break;
        case bsp::keypad_backlight::State::activeMode:
            action = bsp::keypad_backlight::Action::turnOnActiveMode;
            break;
        case bsp::keypad_backlight::State::off:
            action = bsp::keypad_backlight::Action::turnOff;
            break;
        }
        bus.sendUnicast(std::make_shared<sevm::KeypadBacklightMessage>(action), service::name::evt_manager);
    }
} // namespace app::manager
