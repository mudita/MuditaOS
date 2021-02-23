// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-appmgr/model/ApplicationManager.hpp>
#include <service-appmgr/Controller.hpp>

#include <module-apps/messages/AppMessage.hpp>
#include <Common.hpp>
#include <Service/Message.hpp>
#include <Service/Timer.hpp>
#include <SystemManager/SystemManager.hpp>
#include <SystemManager/messages/SystemManagerMessage.hpp>
#include <application-call/ApplicationCall.hpp>
#include <application-special-input/ApplicationSpecialInput.hpp>
#include <application-desktop/ApplicationDesktop.hpp>
#include <i18n/i18n.hpp>
#include <log/log.hpp>
#include <service-appmgr/messages/Message.hpp>
#include <service-evtmgr/EventManager.hpp>
#include <service-gui/ServiceGUI.hpp>
#include <service-eink/ServiceEink.hpp>
#include <service-gui/Common.hpp>
#include <service-desktop/DesktopMessages.hpp>

#include <algorithm>
#include <limits>
#include <utility>

#include <module-utils/Utils.hpp>
#include <module-utils/time/DateAndTimeSettings.hpp>
#include <module-services/service-db/agents/settings/SystemSettings.hpp>
#include <service-appmgr/messages/DOMRequest.hpp>

// Auto phone lock disabled for now till the times when it's debugged
// #define AUTO_PHONE_LOCK_ENABLED
namespace app::manager
{
    namespace
    {
        constexpr auto default_application_locktime_ms = 30000;
        constexpr auto shutdown_delay_ms               = 500;

        constexpr auto timerBlock         = "BlockTimer";
        constexpr auto timerShutdownDelay = "ShutdownDelay";
    } // namespace

    ApplicationManagerBase::ApplicationManagerBase(std::vector<std::unique_ptr<app::ApplicationLauncher>> &&launchers)
        : applications{std::move(launchers)}
    {}

    void ApplicationManagerBase::setState(State _state) noexcept
    {
        state = _state;
    }

    void ApplicationManagerBase::pushApplication(const ApplicationName &name)
    {
        stack.push_front(name);
    }

    void ApplicationManagerBase::popApplication()
    {
        if (!stack.empty()) {
            stack.pop_front();
        }
    }

    void ApplicationManagerBase::clearStack()
    {
        stack.clear();
    }

    auto ApplicationManagerBase::getFocusedApplication() const noexcept -> ApplicationHandle *
    {
        for (const auto &appName : stack) {
            if (auto app = getApplication(appName);
                app != nullptr && app->state() == app::Application::State::ACTIVE_FORGROUND) {
                return app;
            }
        }
        return nullptr;
    }

    auto ApplicationManagerBase::getLaunchingApplication() const noexcept -> ApplicationHandle *
    {
        if (stack.empty()) {
            return nullptr;
        }
        auto app = getApplication(stack.front());
        return app->state() != app::Application::State::ACTIVE_FORGROUND ? app : nullptr;
    }

    auto ApplicationManagerBase::getPreviousApplication() const noexcept -> ApplicationHandle *
    {
        if (stack.size() < 2) {
            return nullptr;
        }
        return getApplication(stack[1]);
    }

    auto ApplicationManagerBase::getApplication(const ApplicationName &name) const noexcept -> ApplicationHandle *
    {
        return applications.findByName(name);
    }

    ApplicationManager::ApplicationManager(const ApplicationName &serviceName,
                                           std::vector<std::unique_ptr<app::ApplicationLauncher>> &&launchers,
                                           const ApplicationName &_rootApplicationName)
        : Service{serviceName}, ApplicationManagerBase(std::move(launchers)), rootApplicationName{_rootApplicationName},
          actionsRegistry{[this](ActionEntry &action) { return handleAction(action); }},
          blockingTimer{std::make_unique<sys::Timer>(
              timerBlock, this, std::numeric_limits<sys::ms>::max(), sys::Timer::Type::SingleShot)},
          shutdownDelay{std::make_unique<sys::Timer>(timerShutdownDelay, this, shutdown_delay_ms)},
          settings(std::make_unique<settings::Settings>(this)),
          phoneModeObserver(std::make_unique<sys::phone_modes::Observer>())
    {
        bus.channels.push_back(sys::BusChannel::PhoneModeChanges);
        registerMessageHandlers();
        blockingTimer->connect([this](sys::Timer &) { onPhoneLocked(); });
    }

    sys::ReturnCodes ApplicationManager::InitHandler()
    {
        blockingTimer->setInterval(default_application_locktime_ms);
        utils::localize.setFallbackLanguage(utils::localize.DefaultLanguage);
        settings->registerValueChange(
            settings::SystemProperties::displayLanguage,
            [this](std::string value) { displayLanguageChanged(value); },
            settings::SettingsScope::Global);
        settings->registerValueChange(
            settings::SystemProperties::inputLanguage,
            [this](std::string value) { inputLanguageChanged(value); },
            settings::SettingsScope::Global);
        settings->registerValueChange(
            settings::SystemProperties::lockTime,
            [this](std::string value) { lockTimeChanged(value); },
            settings::SettingsScope::Global);
        settings->registerValueChange(
            ::settings::SystemProperties::automaticDateAndTimeIsOn,
            [this](std::string value) {
                utils::dateAndTimeSettings.setAutomaticDateAndTimeOn(utils::getNumericValue<bool>(value));
            },
            ::settings::SettingsScope::Global);
        settings->registerValueChange(
            ::settings::SystemProperties::automaticTimeZoneIsOn,
            [this](std::string value) {
                utils::dateAndTimeSettings.setAutomaticTimeZoneOn(utils::getNumericValue<bool>(value));
            },
            ::settings::SettingsScope::Global);
        settings->registerValueChange(
            ::settings::SystemProperties::timeFormat,
            [this](std::string value) {
                utils::dateAndTimeSettings.setTimeFormat(
                    static_cast<utils::time::Locale::TimeFormat>(utils::getNumericValue<unsigned int>(value)));
            },
            ::settings::SettingsScope::Global);
        settings->registerValueChange(
            ::settings::SystemProperties::dateFormat,
            [this](std::string value) {
                utils::dateAndTimeSettings.setDateFormat(
                    static_cast<utils::time::Locale::DateFormat>(utils::getNumericValue<unsigned int>(value)));
            },
            ::settings::SettingsScope::Global);

        startBackgroundApplications();
        if (auto app = getApplication(rootApplicationName); app != nullptr) {
            Controller::sendAction(this, actions::Home);
        }

        return sys::ReturnCodes::Success;
    }

    void ApplicationManager::suspendSystemServices()
    {
        sys::SystemManager::SuspendService(service::name::gui, this);
        sys::SystemManager::SuspendService(service::name::eink, this);
    }

    void ApplicationManager::startBackgroundApplications()
    {
        for (const auto &name : std::vector<ApplicationName>{app::name_call, app::special_input}) {
            if (auto app = getApplication(name); app != nullptr) {
                app->runInBackground(this);
            }
        }
    }

    sys::ReturnCodes ApplicationManager::DeinitHandler()
    {
        settings->unregisterValueChange();
        closeApplications();
        closeServices();
        return sys::ReturnCodes::Success;
    }

    auto ApplicationManager::DataReceivedHandler([[maybe_unused]] sys::DataMessage *msgl,
                                                 [[maybe_unused]] sys::ResponseMessage *resp) -> sys::MessagePointer
    {
        return std::make_shared<sys::ResponseMessage>();
    }

    void ApplicationManager::registerMessageHandlers()
    {
        phoneModeObserver->connect(this);
        phoneModeObserver->subscribe(
            [](sys::phone_modes::PhoneMode phoneMode, sys::phone_modes::Tethering tetheringMode) {
                LOG_INFO("Phone mode changed.");
            });

        connect(typeid(ApplicationStatusRequest), [this](sys::Message *request) {
            auto msg = static_cast<ApplicationStatusRequest *>(request);
            return std::make_shared<ApplicationStatusResponse>(msg->checkAppName,
                                                               getApplication(msg->checkAppName) != nullptr);
        });
        connect(typeid(PowerSaveModeInitRequest), [this](sys::Message *) {
            handlePowerSavingModeInit();
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(PreventBlockingRequest), [this](sys::Message *) {
            blockingTimer->reload();
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
            return std::make_shared<GetCurrentDisplayLanguageResponse>(utils::localize.getDisplayLanguage());
        });
        connect(typeid(InputLanguageChangeRequest), [this](sys::Message *request) {
            auto msg = static_cast<InputLanguageChangeRequest *>(request);
            handleInputLanguageChange(msg);
            return msgHandled();
        });
        connect(typeid(AutomaticDateAndTimeIsOnChangeRequest), [this](sys::Message *request) {
            auto msg = static_cast<AutomaticDateAndTimeIsOnChangeRequest *>(request);
            handleAutomaticDateAndTimeChange(msg);
            return msgHandled();
        });
        connect(typeid(AutomaticTimeZoneIsOnChangeRequest), [this](sys::Message *request) {
            auto msg = static_cast<AutomaticTimeZoneIsOnChangeRequest *>(request);
            handleAutomaticTimeZoneChange(msg);
            return msgHandled();
        });
        connect(typeid(TimeFormatChangeRequest), [this](sys::Message *request) {
            auto msg = static_cast<TimeFormatChangeRequest *>(request);
            handleTimeFormatChange(msg);
            return msgHandled();
        });
        connect(typeid(DateFormatChangeRequest), [this](sys::Message *request) {
            auto msg = static_cast<DateFormatChangeRequest *>(request);
            handleDateFormatChange(msg);
            return msgHandled();
        });
        connect(typeid(ShutdownRequest), [this](sys::Message *) {
            closeApplications();
            closeServices();
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(ActionRequest), [this](sys::Message *request) {
            auto actionMsg = static_cast<ActionRequest *>(request);
            handleActionRequest(actionMsg);
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(ActionHandledResponse), [this](sys::Message *response) {
            actionsRegistry.finished();
            return nullptr;
        });
        connect(typeid(GetCurrentDisplayLanguageRequest), [&](sys::Message *request) {
            return std::make_shared<GetCurrentDisplayLanguageResponse>(utils::localize.getDisplayLanguage());
        });
        connect(typeid(UpdateInProgress), [this](sys::Message *) {
            closeApplicationsOnUpdate();
            return msgHandled();
        });

        connect(typeid(app::manager::DOMRequest), [&](sys::Message *request) { return handleDOMRequest(request); });

        auto convertibleToActionHandler = [this](sys::Message *request) { return handleMessageAsAction(request); };
        connect(typeid(CellularSimRequestPinMessage), convertibleToActionHandler);
        connect(typeid(CellularSimRequestPukMessage), convertibleToActionHandler);
        connect(typeid(CellularUnlockSimMessage), convertibleToActionHandler);
        connect(typeid(CellularBlockSimMessage), convertibleToActionHandler);
        connect(typeid(CellularDisplayCMEMessage), convertibleToActionHandler);
        connect(typeid(CellularMMIResultMessage), convertibleToActionHandler);
        connect(typeid(CellularMMIResponseMessage), convertibleToActionHandler);
        connect(typeid(CellularMMIPushMessage), convertibleToActionHandler);
        connect(typeid(CellularNoSimNotification), convertibleToActionHandler);
        connect(typeid(CellularNotAnEmergencyNotification), convertibleToActionHandler);
        connect(typeid(sys::CriticalBatteryLevelNotification), convertibleToActionHandler);
        connect(typeid(sys::SystemBrownoutMesssage), convertibleToActionHandler);
        connect(typeid(CellularSmsNoSimRequestMessage), convertibleToActionHandler);
        connect(typeid(sdesktop::passcode::ScreenPasscodeRequest), convertibleToActionHandler);
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
            app::Application::messageSwitchApplication(this, app.name(), app.switchWindow, std::move(app.switchData));
        }
        else {
            LOG_INFO("Starting application %s", app.name().c_str());
            app.run(this);
        }
        return true;
    }

    auto ApplicationManager::closeServices() -> bool
    {
        closeService(service::name::gui);
        closeService(service::name::eink);
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

    void ApplicationManager::closeService(const std::string &name)
    {
        bool ret = sys::SystemManager::DestroyService(name, this);
        if (ret) {
            LOG_INFO("Service/Application %s closed", name.c_str());
        }
        else {
            LOG_FATAL("Service/Application %s is still running", name.c_str());
        }
    }

    void ApplicationManager::closeApplication(ApplicationHandle *application)
    {
        if (application == nullptr) {
            return;
        }

        closeService(application->name());
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
            return true;
        }

        if (app->name() == currentlyFocusedApp->name()) {
            LOG_WARN("Failed to return to currently focused application.");
            return false;
        }

        onApplicationSwitch(*app, std::move(msg->getData()), msg->getWindow());
        const bool isFocusedAppCloseable =
            closeCurrentlyFocusedApp && currentlyFocusedApp->closeable() && !currentlyFocusedApp->blockClosing;
        requestApplicationClose(*currentlyFocusedApp, isFocusedAppCloseable);
        return true;
    }

    void ApplicationManager::onApplicationSwitch(ApplicationHandle &app,
                                                 std::unique_ptr<gui::SwitchData> &&data,
                                                 std::string targetWindow)
    {
        if (app.name() == rootApplicationName) {
            clearStack();
        }
        pushApplication(app.name());
        app.switchData   = std::move(data);
        app.switchWindow = std::move(targetWindow);
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

    bool ApplicationManager::handleAction(ActionEntry &action)
    {
        switch (action.actionId) {
        case actions::Home:
            return handleHomeAction(action);
        case actions::Launch:
            return handleLaunchAction(action);
        case actions::CloseSystem:
            return handleCloseSystem();
        default:
            return handleCustomAction(action);
        }
    }

    auto ApplicationManager::handleHomeAction(ActionEntry &action) -> bool
    {
        action.setTargetApplication(rootApplicationName);
        SwitchRequest switchRequest(ServiceName, rootApplicationName, gui::name::window::main_window, nullptr);
        return handleSwitchApplication(&switchRequest);
    }

    auto ApplicationManager::handleLaunchAction(ActionEntry &action) -> bool
    {
        auto launchParams = static_cast<ApplicationLaunchData *>(action.params.get());
        auto targetApp = getApplication(launchParams->getTargetApplicationName());
        if (targetApp == nullptr || !targetApp->handles(actions::Launch)) {
            return false;
        }

        action.setTargetApplication(targetApp->name());
        SwitchRequest switchRequest(ServiceName, targetApp->name(), gui::name::window::main_window, nullptr);
        return handleSwitchApplication(&switchRequest);
    }

    auto ApplicationManager::handleCloseSystem() -> bool
    {
        shutdownDelay->connect([&](sys::Timer &) { sys::SystemManager::CloseSystem(this); });
        shutdownDelay->start();

        return true;
    }

    auto ApplicationManager::handleCustomAction(ActionEntry &action) -> bool
    {
        const auto actionHandlers = applications.findByAction(action.actionId);
        if (actionHandlers.empty()) {
            LOG_ERROR("No applications handling action #%d.", action.actionId);
            return false;
        }
        if (actionHandlers.size() > 1) {
            LOG_FATAL("Choosing amongst multiple action handler applications is not yet implemented.");
            return false;
        }

        const auto targetApp = actionHandlers.front();
        action.setTargetApplication(targetApp->name());
        auto &actionParams = action.params;
        if (targetApp->state() != ApplicationHandle::State::ACTIVE_FORGROUND) {
            const auto focusedAppClose = !(actionParams && actionParams->disableAppClose);
            SwitchRequest switchRequest(
                ServiceName, targetApp->name(), targetApp->switchWindow, std::move(targetApp->switchData));
            return handleSwitchApplication(&switchRequest, focusedAppClose);
        }

        app::Application::requestAction(this, targetApp->name(), action.actionId, std::move(actionParams));
        return true;
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
            LOG_WARN("Failed to return to currently focused application.");
            return false;
        }

        LOG_DEBUG("Switch applications: [%s][%s](%s) -> [%s][%s](%s)",
                  currentlyFocusedApp->name().c_str(),
                  currentlyFocusedApp->switchWindow.c_str(),
                  app::Application::stateStr(currentlyFocusedApp->state()),
                  previousApp->name().c_str(),
                  previousApp->switchWindow.c_str(),
                  app::Application::stateStr(previousApp->state()));

        onApplicationSwitchToPrev(*previousApp, std::move(msg->getData()));
        requestApplicationClose(*currentlyFocusedApp, currentlyFocusedApp->closeable());
        return true;
    }

    void ApplicationManager::onApplicationSwitchToPrev(ApplicationHandle &previousApp,
                                                       std::unique_ptr<gui::SwitchData> &&data)
    {
        popApplication();
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
            app::Application::messageSwitchApplication(this, app.name(), app.switchWindow, std::move(app.switchData));
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

        if (requestedLanguage == utils::localize.getDisplayLanguage()) {
            LOG_WARN("The selected language is already set. Ignore.");
            return false;
        }
        settings->setValue(
            settings::SystemProperties::displayLanguage, requestedLanguage, settings::SettingsScope::Global);
        utils::localize.setDisplayLanguage(requestedLanguage);
        rebuildActiveApplications();
        return true;
    }

    auto ApplicationManager::handleInputLanguageChange(app::manager::InputLanguageChangeRequest *msg) -> bool
    {
        const auto &requestedLanguage = msg->getLanguage();

        if (requestedLanguage == utils::localize.getInputLanguage()) {
            LOG_WARN("The selected language is already set. Ignore.");
            return false;
        }
        settings->setValue(
            settings::SystemProperties::inputLanguage, requestedLanguage, settings::SettingsScope::Global);
        utils::localize.setInputLanguage(requestedLanguage);
        return true;
    }

    auto ApplicationManager::handleAutomaticDateAndTimeChange(AutomaticDateAndTimeIsOnChangeRequest *msg) -> bool
    {
        if (utils::dateAndTimeSettings.isAutomaticDateAndTimeOn() == msg->isOn) {
            LOG_WARN("The selected value is already set. Ignore.");
            return false;
        }
        settings->setValue(settings::SystemProperties::automaticDateAndTimeIsOn,
                           std::to_string(msg->isOn),
                           settings::SettingsScope::Global);
        utils::dateAndTimeSettings.setAutomaticDateAndTimeOn(msg->isOn);
        return true;
    }

    auto ApplicationManager::handleAutomaticTimeZoneChange(AutomaticTimeZoneIsOnChangeRequest *msg) -> bool
    {
        if (utils::dateAndTimeSettings.isAutomaticTimeZoneOn() == msg->isOn) {
            LOG_WARN("The selected value is already set. Ignore.");
            return false;
        }
        settings->setValue(settings::SystemProperties::automaticTimeZoneIsOn,
                           std::to_string(msg->isOn),
                           settings::SettingsScope::Global);
        utils::dateAndTimeSettings.setAutomaticTimeZoneOn(msg->isOn);
        return true;
    }

    auto ApplicationManager::handleTimeFormatChange(TimeFormatChangeRequest *msg) -> bool
    {
        if (utils::dateAndTimeSettings.getTimeFormat() == msg->timeFormat) {
            LOG_WARN("The selected value is already set. Ignore.");
            return false;
        }
        settings->setValue(settings::SystemProperties::timeFormat,
                           std::to_string(static_cast<unsigned>(msg->timeFormat)),
                           settings::SettingsScope::Global);
        utils::dateAndTimeSettings.setTimeFormat(msg->timeFormat);
        return true;
    }

    auto ApplicationManager::handleDateFormatChange(DateFormatChangeRequest *msg) -> bool
    {
        if (utils::dateAndTimeSettings.getDateFormat() == msg->dateFormat) {
            LOG_WARN("The selected value is already set. Ignore.");
            return false;
        }
        settings->setValue(settings::SystemProperties::dateFormat,
                           std::to_string(static_cast<unsigned>(msg->dateFormat)),
                           settings::SettingsScope::Global);
        utils::dateAndTimeSettings.setDateFormat(msg->dateFormat);
        return true;
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
        if (app.closeable()) {
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
#ifdef AUTO_PHONE_LOCK_ENABLED
        LOG_INFO("Screen lock timer triggered.");
        blockingTimer->stop();

        auto focusedApp = getFocusedApplication();
        if (focusedApp == nullptr || focusedApp->preventsBlocking()) {
            blockingTimer->reload();
            return;
        }

        if (focusedApp->name() == rootApplicationName) {
            app::Application::messageSwitchApplication(
                this,
                rootApplicationName,
                gui::name::window::main_window,
                std::make_unique<gui::LockPhoneData>(gui::LockPhoneData::Request::NoPin));
        }
        else {
            focusedApp->blockClosing = true;
            std::unique_ptr<gui::LockPhoneData> data =
                std::make_unique<gui::LockPhoneData>(gui::LockPhoneData::Request::NoPin);
            data->setPrevApplication(focusedApp->name());
            Controller::switchApplication(this, rootApplicationName, gui::name::window::main_window, std::move(data));
        }
#endif
    }
    void ApplicationManager::displayLanguageChanged(std::string value)
    {
        if (value.empty()) {
            return;
        }
        utils::localize.setDisplayLanguage(value);
        rebuildActiveApplications();
    }
    void ApplicationManager::lockTimeChanged(std::string value)
    {
        if (value.empty()) {
            return;
        }
        blockingTimer->setInterval(utils::getNumericValue<unsigned int>(value));
        //?any additional action needed here?
    }
    void ApplicationManager::inputLanguageChanged(std::string value)
    {
        if (value.empty()) {
            return;
        }
        utils::localize.setInputLanguage(value);
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
} // namespace app::manager
