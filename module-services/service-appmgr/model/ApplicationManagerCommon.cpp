// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationManagerCommon.hpp"
#include "ApplicationStatus.hpp"
#include "Controller.hpp"
#include "DOMRequest.hpp"
#include "FinishRequest.hpp"
#include "Message.hpp"

#include <Service/Message.hpp>
#include <SystemManager/SystemManagerCommon.hpp>
#include <SystemManager/messages/SystemManagerMessage.hpp>
#include <apps-common/messages/OnBoardingMessages.hpp>
#include <apps-common/messages/AppMessage.hpp>
#include <apps-common/actions/AlarmTriggeredAction.hpp>
#include <i18n/i18n.hpp>
#include <log/log.hpp>
#include <service-audio/AudioMessage.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <module-utils/Utils.hpp>
#include <service-appmgr/Constants.hpp>
#include <service-desktop/Constants.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <service-eink/ServiceEink.hpp>
#include <service-evtmgr/EventManagerCommon.hpp>

#include <algorithm>
#include <utility>

namespace app::manager
{
    namespace
    {
        constexpr auto ApplicationManagerStackDepth = 3072;
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
                app != nullptr && app->state() == app::ApplicationCommon::State::ACTIVE_FORGROUND) {
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
        return app->state() != app::ApplicationCommon::State::ACTIVE_FORGROUND ? app : nullptr;
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

    ApplicationManagerCommon::ApplicationManagerCommon(
        const ApplicationName &serviceName,
        std::vector<std::unique_ptr<app::ApplicationLauncher>> &&launchers,
        const ApplicationName &_rootApplicationName)
        : Service{serviceName, {}, ApplicationManagerStackDepth},
          ApplicationManagerBase(std::move(launchers)), rootApplicationName{_rootApplicationName},
          actionsRegistry{[this](ActionEntry &action) { return handleAction(action); }},
          settings(std::make_shared<settings::Settings>())
    {
        bus.channels.push_back(sys::BusChannel::ServiceAudioNotifications);
        bus.channels.push_back(sys::BusChannel::ServiceDBNotifications);
    }

    sys::ReturnCodes ApplicationManagerCommon::InitHandler()
    {
        settings->init(service::ServiceProxy(shared_from_this()));
        utils::setDisplayLanguage(
            settings->getValue(settings::SystemProperties::displayLanguage, settings::SettingsScope::Global));
        settings->registerValueChange(
            settings::SystemProperties::displayLanguage,
            [this](std::string value) { displayLanguageChanged(std::move(value)); },
            settings::SettingsScope::Global);
        settings->registerValueChange(
            settings::SystemProperties::inputLanguage,
            [this](std::string value) { inputLanguageChanged(std::move(value)); },
            settings::SettingsScope::Global);

        bus.sendUnicast(std::make_unique<CheckIfStartAllowedMessage>(), service::name::system_manager);

        return sys::ReturnCodes::Success;
    }

    void ApplicationManagerCommon::handleStart(StartAllowedMessage *msg)
    {
        if (auto app = getApplication(rootApplicationName); app != nullptr) {
            Controller::sendAction(this, actions::Home);
        }
    }

    void ApplicationManagerCommon::suspendSystemServices()
    {
        sys::SystemManagerCommon::SuspendService(service::name::gui, this);
        sys::SystemManagerCommon::SuspendService(service::name::eink, this);
    }

    sys::ReturnCodes ApplicationManagerCommon::DeinitHandler()
    {
        settings->deinit();
        closeApplications();
        return sys::ReturnCodes::Success;
    }

    auto ApplicationManagerCommon::ProcessCloseReason(sys::CloseReason closeReason) -> void
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
            [[fallthrough]];
        case sys::CloseReason::FactoryReset:
            break;
        }
        handleActionRequest(&act);
        sendCloseReadyMessage(this);
    }

    auto ApplicationManagerCommon::DataReceivedHandler([[maybe_unused]] sys::DataMessage *msgl,
                                                       [[maybe_unused]] sys::ResponseMessage *resp)
        -> sys::MessagePointer
    {
        return std::make_shared<sys::ResponseMessage>();
    }

    void ApplicationManagerCommon::registerMessageHandlers()
    {
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
        connect(typeid(FinalizingClose), [this](sys::Message *request) {
            auto msg = static_cast<FinalizingClose *>(request);
            handleFinalizingClose(msg);
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

        connect(typeid(onBoarding::FinalizeOnBoarding),
                [&](sys::Message *request) -> sys::MessagePointer { return handleOnBoardingFinalize(); });

        connect(typeid(sdesktop::developerMode::DeveloperModeRequest),
                [&](sys::Message *request) -> sys::MessagePointer { return handleDeveloperModeRequest(request); });

        connect(typeid(app::manager::DOMRequest), [&](sys::Message *request) { return handleDOMRequest(request); });

        auto convertibleToActionHandler = [this](sys::Message *request) { return handleMessageAsAction(request); };
        connect(typeid(sys::CriticalBatteryLevelNotification), convertibleToActionHandler);
        connect(typeid(VolumeChanged), convertibleToActionHandler);
        connect(typeid(app::actions::AlarmTriggeredAction), convertibleToActionHandler);
    }

    sys::ReturnCodes ApplicationManagerCommon::SwitchPowerModeHandler(const sys::ServicePowerMode mode)
    {
        LOG_INFO("Power mode: %s", c_str(mode));

        switch (mode) {
        case sys::ServicePowerMode ::Active:
            sys::SystemManagerCommon::ResumeService(service::name::eink, this);
            sys::SystemManagerCommon::ResumeService(service::name::gui, this);
            break;
        case sys::ServicePowerMode ::SuspendToRAM:
            [[fallthrough]];
        case sys::ServicePowerMode ::SuspendToNVM:
            suspendSystemServices();
            break;
        }
        return sys::ReturnCodes::Success;
    }

    auto ApplicationManagerCommon::startApplication(ApplicationHandle &app) -> bool
    {
        if (app.state() == ApplicationHandle::State::ACTIVE_BACKGROUND) {
            LOG_INFO("Switching focus to application [%s] (window [%s])", app.name().c_str(), app.switchWindow.c_str());
            setState(State::AwaitingFocusConfirmation);
            app::ApplicationCommon::messageSwitchApplication(
                this, app.name(), app.switchWindow, std::move(app.switchData), StartupReason::Launch);
            return true;
        }
        return false;
    }

    void ApplicationManagerCommon::startPendingApplicationOnCurrentClose()
    {
        if (const auto launchingApp = getLaunchingApplication();
            launchingApp != nullptr && getState() == State::AwaitingCloseConfirmation) {
            startApplication(*launchingApp);
        }
    }

    auto ApplicationManagerCommon::closeApplications() -> bool
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

    void ApplicationManagerCommon::closeNoLongerNeededApplications()
    {
        for (const auto &app : getApplications()) {
            if (app->started() && app->closeable() && !stack.contains(app->name())) {
                closeApplication(app.get());
                app->setState(ApplicationHandle::State::DEACTIVATED);
            }
        }
    }

    void ApplicationManagerCommon::closeApplication(ApplicationHandle *application)
    {
        if (application == nullptr) {
            return;
        }

        if (sys::SystemManagerCommon::DestroyApplication(application->name(), this)) {
            LOG_INFO("Application %s closed", application->name().c_str());
        }
        else {
            LOG_FATAL("Application %s is still running", application->name().c_str());
        }
        application->close();
    }

    auto ApplicationManagerCommon::handlePowerSavingModeInit() -> bool
    {
        LOG_INFO("Going to suspend mode");
        return true;
    }

    auto ApplicationManagerCommon::handleSwitchApplication(SwitchRequest *msg, bool closeCurrentlyFocusedApp) -> bool
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
                  app::ApplicationCommon::stateStr(currentlyFocusedApp->state()),
                  app->name().c_str(),
                  app->switchWindow.c_str(),
                  app::ApplicationCommon::stateStr(app->state()));

        stack.front().isCloseable = closeCurrentlyFocusedApp;
        onApplicationSwitch(*app, std::move(msg->getData()), msg->getWindow());
        if (app->name() == currentlyFocusedApp->name()) {
            // Switch window only.
            app::ApplicationCommon::messageSwitchApplication(
                this, app->name(), app->switchWindow, std::move(app->switchData), StartupReason::Launch);
            return false;
        }

        requestApplicationClose(*currentlyFocusedApp, isApplicationCloseable(currentlyFocusedApp));
        return true;
    }

    void ApplicationManagerCommon::onApplicationSwitch(ApplicationHandle &nextApp,
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

    void ApplicationManagerCommon::requestApplicationClose(ApplicationHandle &app, bool isCloseable)
    {
        if (isCloseable) {
            LOG_INFO("Closing application %s", app.name().c_str());
            setState(State::AwaitingCloseConfirmation);
            app::ApplicationCommon::messageCloseApplication(this, app.name());
        }
        else {
            LOG_INFO("Application %s is about to lose focus.", app.name().c_str());
            setState(State::AwaitingLostFocusConfirmation);
            app::ApplicationCommon::messageApplicationLostFocus(this, app.name());
        }
    }

    void ApplicationManagerCommon::handleActionRequest(ActionRequest *actionMsg)
    {
        ActionEntry entry{actionMsg->getAction(), std::move(actionMsg->getData())};
        actionsRegistry.enqueue(std::move(entry));
    }

    ActionProcessStatus ApplicationManagerCommon::handleAction(ActionEntry &action)
    {
        switch (action.actionId) {
        case actions::Home:
            return handleHomeAction(action);
        case actions::Launch:
            return handleLaunchAction(action);
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

    auto ApplicationManagerCommon::handleHomeAction(ActionEntry &action) -> ActionProcessStatus
    {
        action.setTargetApplication(resolveHomeApplication());

        SwitchRequest switchRequest(service::name::appmgr, resolveHomeApplication(), resolveHomeWindow(), nullptr);
        return handleSwitchApplication(&switchRequest) ? ActionProcessStatus::Accepted : ActionProcessStatus::Dropped;
    }

    auto ApplicationManagerCommon::resolveHomeWindow() -> std::string
    {
        return gui::name::window::main_window;
    }

    auto ApplicationManagerCommon::handleOnBoardingFinalize() -> sys::MessagePointer
    {
        settings->setValue(settings::SystemProperties::onboardingDone, utils::to_string(true));
        app::manager::Controller::sendAction(this, app::manager::actions::Home);
        return sys::msgHandled();
    }

    auto ApplicationManagerCommon::checkOnBoarding() -> bool
    {
        return not utils::getNumericValue<bool>(settings->getValue(settings::SystemProperties::onboardingDone));
    }

    auto ApplicationManagerCommon::handleLaunchAction(ActionEntry &action) -> ActionProcessStatus
    {
        auto launchParams = static_cast<ApplicationLaunchData *>(action.params.get());
        auto targetApp    = getApplication(launchParams->getTargetApplicationName());
        if (targetApp == nullptr || !targetApp->handles(actions::Launch)) {
            return ActionProcessStatus::Dropped;
        }

        targetApp->startupReason = StartupReason::Launch;
        action.setTargetApplication(targetApp->name());
        SwitchRequest switchRequest(service::name::appmgr, targetApp->name(), gui::name::window::main_window, nullptr);
        return handleSwitchApplication(&switchRequest) ? ActionProcessStatus::Accepted : ActionProcessStatus::Dropped;
    }

    auto ApplicationManagerCommon::handleActionOnActiveApps(ActionEntry &action) -> ActionProcessStatus
    {
        const auto &targetName = action.target;
        auto targetApp         = getApplication(targetName);
        if (targetApp == nullptr || (!targetApp->handles(action.actionId))) {
            return ActionProcessStatus::Dropped;
        }

        if (targetApp->state() == ApplicationHandle::State::ACTIVE_FORGROUND ||
            targetApp->state() == ApplicationHandle::State::ACTIVE_BACKGROUND) {
            app::ApplicationCommon::requestAction(this, targetName, action.actionId, std::move(action.params));
            return ActionProcessStatus::Accepted;
        }
        return ActionProcessStatus::Skipped;
    }

    auto ApplicationManagerCommon::handleActionOnFocusedApp(ActionEntry &action) -> ActionProcessStatus
    {
        auto targetApp = getFocusedApplication();
        if (targetApp == nullptr) {
            return ActionProcessStatus::Skipped;
        }
        action.setTargetApplication(targetApp->name());
        auto &params = action.params;
        app::ApplicationCommon::requestAction(this, targetApp->name(), action.actionId, std::move(params));
        return ActionProcessStatus::Accepted;
    }

    auto ApplicationManagerCommon::handleCustomAction(ActionEntry &action) -> ActionProcessStatus
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
            app::ApplicationCommon::requestAction(this, targetApp->name(), action.actionId, std::move(actionParams));
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

        const auto focusedAppClose = !(actionPolicy(action.actionId) == Reaction::KeepFocusedAppInBackground ||
                                       (actionParams && actionParams->disableAppClose));
        SwitchRequest switchRequest(
            service::name::appmgr, targetApp->name(), targetApp->switchWindow, std::move(targetApp->switchData));
        return handleSwitchApplication(&switchRequest, focusedAppClose) ? ActionProcessStatus::Accepted
                                                                        : ActionProcessStatus::Skipped;
    }

    auto ApplicationManagerCommon::handleCustomActionOnBackgroundApp(ApplicationHandle *app, ActionEntry &action)
        -> ActionProcessStatus
    {
        if (const auto actionFlag = app->actionFlag(action.actionId);
            actionFlag == actions::ActionFlag::AcceptWhenInBackground) {
            app::ApplicationCommon::requestAction(this, app->name(), action.actionId, std::move(action.params));
            return ActionProcessStatus::Accepted;
        }
        return ActionProcessStatus::Dropped;
    }

    auto ApplicationManagerCommon::handleSwitchBack(SwitchBackRequest *msg) -> bool
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
            app::ApplicationCommon::messageSwitchBack(this, currentlyFocusedApp->name());
            return true;
        }

        LOG_DEBUG("Switch applications: [%s][%s](%s) -> [%s][%s](%s)",
                  currentlyFocusedApp->name().c_str(),
                  currentlyFocusedApp->switchWindow.c_str(),
                  app::ApplicationCommon::stateStr(currentlyFocusedApp->state()),
                  previousApp->name().c_str(),
                  previousApp->switchWindow.c_str(),
                  app::ApplicationCommon::stateStr(previousApp->state()));

        onApplicationSwitchToPrev(*previousApp, std::move(msg->getData()));
        requestApplicationClose(*currentlyFocusedApp, isApplicationCloseable(currentlyFocusedApp));
        return true;
    }

    void ApplicationManagerCommon::onApplicationSwitchToPrev(ApplicationHandle &previousApp,
                                                             std::unique_ptr<gui::SwitchData> &&data)
    {
        stack.pop();
        previousApp.switchData = std::move(data);
    }

    auto ApplicationManagerCommon::handleInitApplication(ApplicationInitialised *msg) -> bool
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

    void ApplicationManagerCommon::onApplicationInitialised(ApplicationHandle &app, StartInBackground startInBackground)
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

            app::ApplicationCommon::messageSwitchApplication(
                this, app.name(), app.switchWindow, std::move(app.switchData), launchingApp->startupReason);
        }
    }

    void ApplicationManagerCommon::onApplicationInitFailure(ApplicationHandle &app)
    {
        LOG_ERROR("Failed to initialise %s: Application internal error.", app.name().c_str());
        Controller::switchBack(this);
    }

    auto ApplicationManagerCommon::handleDisplayLanguageChange(app::manager::DisplayLanguageChangeRequest *msg) -> bool
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

    auto ApplicationManagerCommon::handleInputLanguageChange(app::manager::InputLanguageChangeRequest *msg) -> bool
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

    void ApplicationManagerCommon::rebuildActiveApplications()
    {
        for (const auto &app : getApplications()) {
            if (app && app->valid()) {
                if (const auto appState = app->state(); appState == ApplicationHandle::State::ACTIVE_FORGROUND ||
                                                        appState == ApplicationHandle::State::ACTIVE_BACKGROUND) {
                    app::ApplicationCommon::messageRebuildApplication(this, app->name());
                }
            }
        }
    }

    auto ApplicationManagerCommon::handleSwitchConfirmation(SwitchConfirmation *msg) -> bool
    {
        auto senderApp = getApplication(msg->getSenderName());
        if (senderApp == nullptr) {
            LOG_ERROR("Failed to switch to %s. No such application.", msg->getSenderName().c_str());
            return false;
        }
        LOG_INFO("Switch confirmed by %s (%s).",
                 senderApp->name().c_str(),
                 app::ApplicationCommon::stateStr(senderApp->state()));
        return onSwitchConfirmed(*senderApp);
    }

    auto ApplicationManagerCommon::onSwitchConfirmed(ApplicationHandle &app) -> bool
    {
        if (getState() == State::AwaitingFocusConfirmation || getState() == State::Running) {
            app.setState(ApplicationHandle::State::ACTIVE_FORGROUND);
            setState(State::Running);
            EventManagerCommon::messageSetApplication(this, app.name());
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

    void ApplicationManagerCommon::onLaunchFinished(ApplicationHandle &app)
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
            app::ApplicationCommon::requestAction(this, app.name(), action->actionId, std::move(params));
            break;
        }
        }
    }

    void ApplicationManagerCommon::handleFinalizingClose(FinalizingClose *msg)
    {
        auto senderApp = getApplication(msg->getSenderName());
        LOG_DEBUG("Waiting to close application [%s] - finalizing requests", senderApp->name().c_str());

        onFinalizingClose();
    }

    auto ApplicationManagerCommon::handleCloseConfirmation(CloseConfirmation *msg) -> bool
    {
        auto senderApp = getApplication(msg->getSenderName());
        if (senderApp == nullptr) {
            LOG_ERROR("Failed to handle close confirmation from %s: No such application.",
                      msg->getSenderName().c_str());
            return false;
        }
        return onCloseConfirmed(*senderApp);
    }

    void ApplicationManagerCommon::onFinalizingClose()
    {
        startPendingApplicationOnCurrentClose();
    }

    auto ApplicationManagerCommon::onCloseConfirmed(ApplicationHandle &app) -> bool
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

    auto ApplicationManagerCommon::handleMessageAsAction(sys::Message *request) -> std::shared_ptr<sys::ResponseMessage>
    {
        auto actionMsg = dynamic_cast<manager::actions::ConvertibleToAction *>(request);
        if (!actionMsg) {
            return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Failure);
        }
        auto action = actionMsg->toAction();
        handleActionRequest(action.get());

        return std::make_shared<sys::ResponseMessage>();
    }

    void ApplicationManagerCommon::displayLanguageChanged(std::string value)
    {
        if (utils::setDisplayLanguage(value)) {
            rebuildActiveApplications();
        }
    }

    void ApplicationManagerCommon::inputLanguageChanged(std::string value)
    {
        utils::setInputLanguage(value);
    }

    auto ApplicationManagerCommon::handleDOMRequest(sys::Message *request) -> std::shared_ptr<sys::ResponseMessage>
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

    auto ApplicationManagerCommon::handleDeveloperModeRequest(sys::Message *request) -> sys::MessagePointer
    {
        return sys::msgNotHandled();
    };
} // namespace app::manager
