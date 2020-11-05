// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "service-appmgr/ApplicationManager.hpp"

#include <utility>   // for move
#include <algorithm> // for find_if
#include <limits>    // for numeric_limits

#include "service-appmgr/Controller.hpp"                         // for Controller
#include "application-call/ApplicationCall.hpp"                  // for name_call
#include "application-special-input/ApplicationSpecialInput.hpp" // for special_input
#include "Service/Message.hpp"             // for ResponseMessage, DataMessage (ptr only), Message_t
#include "AppMessage.hpp"                  // for AppSwitchWindowMessage
#include "Service/Timer.hpp"               // for Timer, Timer::Type, Timer::Type::SingleShot, ms
#include "service-db/api/DBServiceAPI.hpp" // for DBServiceAPI
#include "service-evtmgr/EventManager.hpp" // for EventManager
#include <service-eink/ServiceEink.hpp>    // for ServiceEink
#include <service-eink/Common.hpp>
#include <service-gui/Common.hpp>
#include "service-gui/ServiceGUI.hpp"      // for ServiceGUI
#include "log/log.hpp"                     // for LOG_INFO, LOG_ERROR, LOG_WARN, LOG_DEBUG, LOG_FATAL
#include "Common.hpp"                      // for ShowMode, ShowMode::GUI_SHOW_INIT
#include "Common/Common.hpp" // for SettingsLanguage, SettingsLanguage::ENGLISH, SettingsLanguage::GERMAN, SettingsLanguage::POLISH, SettingsLanguage::SPANISH
#include "Service/Bus.hpp"   // for Bus
#include "SystemManager/SystemManager.hpp"        // for SystemManager
#include "i18/i18.hpp"                            // for Lang, Lang::En, Lang::De, Lang::Pl, Lang::Sp, i18, localize
#include "service-appmgr/messages/APMMessage.hpp" // for APMCheckApp, APMSwitch, APMRegister, APMConfirmClose, APMConfirmSwitch, Action, APMAction, APMChangeLanguage, APMSwitchPrevApp, APMDelayedClose, APMClose, APMInitPowerSaveMode, APMPreventBlocking

// Auto phone lock disabled for now till the times when it's debugged
// #define AUTO_PHONE_LOCK_ENABLED

namespace app::manager
{
    namespace
    {
        constexpr auto default_application_locktime_ms = 30000;

        utils::Lang toUtilsLanguage(SettingsLanguage language)
        {
            switch (language) {
            case SettingsLanguage::ENGLISH:
                return utils::Lang::En;
            case SettingsLanguage::POLISH:
                return utils::Lang::Pl;
            case SettingsLanguage::GERMAN:
                return utils::Lang::De;
            case SettingsLanguage::SPANISH:
                return utils::Lang::Sp;
            default:
                return utils::Lang::En;
            }
        }

        SettingsLanguage toSettingsLanguage(utils::Lang language)
        {
            switch (language) {
            case utils::Lang::En:
                return SettingsLanguage::ENGLISH;
            case utils::Lang::Pl:
                return SettingsLanguage::POLISH;
            case utils::Lang::De:
                return SettingsLanguage::GERMAN;
            case utils::Lang::Sp:
                return SettingsLanguage::SPANISH;
            default:
                return SettingsLanguage::ENGLISH;
            }
        }
    } // namespace

    ApplicationHandle::ApplicationHandle(std::unique_ptr<app::ApplicationLauncher> &&_launcher)
        : launcher{std::move(_launcher)}
    {}

    auto ApplicationHandle::name() const -> Name
    {
        return launcher ? launcher->getName() : InvalidAppName.data();
    }

    auto ApplicationHandle::state() const noexcept -> State
    {
        return launcher && launcher->handle ? launcher->handle->getState() : State::NONE;
    }

    void ApplicationHandle::setState(State state) noexcept
    {
        if (launcher && launcher->handle) {
            launcher->handle->setState(state);
        }
    }

    auto ApplicationHandle::preventsBlocking() const noexcept -> bool
    {
        return launcher ? launcher->isBlocking() : false;
    }

    auto ApplicationHandle::closeable() const noexcept -> bool
    {
        return launcher ? launcher->isCloseable() : false;
    }

    auto ApplicationHandle::started() const noexcept -> bool
    {
        const auto appState = state();
        return appState == State::ACTIVE_FORGROUND || appState == State::ACTIVE_BACKGROUND ||
               appState == State::ACTIVATING;
    }

    void ApplicationHandle::run(sys::Service *caller)
    {
        if (launcher) {
            launcher->run(caller);
        }
    }

    void ApplicationHandle::runInBackground(sys::Service *caller)
    {
        if (launcher) {
            launcher->runBackground(caller);
        }
    }

    ApplicationManagerBase::ApplicationManagerBase(std::vector<std::unique_ptr<app::ApplicationLauncher>> &&launchers)
    {
        std::vector<std::unique_ptr<app::ApplicationLauncher>> container = std::move(launchers);
        for (auto &&launcher : container) {
            applications.push_back(std::make_unique<ApplicationHandle>(std::move(launcher)));
        }
    }

    void ApplicationManagerBase::setState(State _state) noexcept
    {
        state = _state;
    }

    void ApplicationManagerBase::pushApplication(const ApplicationHandle::Name &name)
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

    auto ApplicationManagerBase::getApplication(const ApplicationHandle::Name &name) const noexcept
        -> ApplicationHandle *
    {
        auto it = std::find_if(
            applications.begin(), applications.end(), [&name](const auto &app) { return app->name() == name; });
        if (it == applications.end()) {
            return nullptr;
        }
        return it->get();
    }

    ApplicationManager::ApplicationManager(const std::string &serviceName,
                                           std::vector<std::unique_ptr<app::ApplicationLauncher>> &&launchers,
                                           const ApplicationHandle::Name &_rootApplicationName)
        : Service{serviceName}, ApplicationManagerBase(std::move(launchers)), rootApplicationName{_rootApplicationName},
          blockingTimer{std::make_unique<sys::Timer>(
              "BlockTimer", this, std::numeric_limits<sys::ms>::max(), sys::Timer::Type::SingleShot)}
    {
        registerMessageHandlers();
        blockingTimer->connect([this](sys::Timer &) { onPhoneLocked(); });
    }

    sys::ReturnCodes ApplicationManager::InitHandler()
    {
        settings = DBServiceAPI::SettingsGet(this);
        blockingTimer->setInterval(settings.lockTime != 0 ? settings.lockTime : default_application_locktime_ms);
        utils::localize.Switch(toUtilsLanguage(settings.language));

        startSystemServices();
        startBackgroundApplications();
        if (auto app = getApplication(rootApplicationName); app != nullptr) {
            Controller::switchApplication(this, rootApplicationName, std::string{}, nullptr);
        }

        return sys::ReturnCodes::Success;
    }

    void ApplicationManager::startSystemServices()
    {
        if (bool ret = sys::SystemManager::CreateService(
                std::make_shared<sgui::ServiceGUI>(service::name::gui, GetName(), gui::Size{480, 600}), this);
            !ret) {
            LOG_ERROR("Failed to initialize GUI service");
        }
        if (bool ret =
                sys::SystemManager::CreateService(std::make_shared<ServiceEink>(service::name::eink, GetName()), this);
            !ret) {
            LOG_ERROR("Failed to initialize EInk service");
        }
    }

    void ApplicationManager::suspendSystemServices()
    {
        sys::SystemManager::SuspendService(service::name::gui, this);
        sys::SystemManager::SuspendService(service::name::eink, this);
    }

    void ApplicationManager::startBackgroundApplications()
    {
        for (const auto &name : std::vector<ApplicationHandle::Name>{app::name_call, app::special_input}) {
            if (auto app = getApplication(name); app != nullptr) {
                app->runInBackground(this);
            }
        }
    }

    sys::ReturnCodes ApplicationManager::DeinitHandler()
    {
        closeApplications();
        closeServices();
        return sys::ReturnCodes::Success;
    }

    auto ApplicationManager::DataReceivedHandler([[maybe_unused]] sys::DataMessage *msgl,
                                                 [[maybe_unused]] sys::ResponseMessage *resp) -> sys::Message_t
    {
        return std::make_shared<sys::ResponseMessage>();
    }

    void ApplicationManager::registerMessageHandlers()
    {
        connect(typeid(APMCheckApp), [this](sys::DataMessage *request, sys::ResponseMessage *) {
            auto msg       = static_cast<APMCheckApp *>(request);
            auto ret       = std::make_shared<APMCheckApp>(GetName(), msg->checkAppName);
            ret->isRunning = getApplication(msg->checkAppName) != nullptr;
            return ret;
        });
        connect(typeid(APMInitPowerSaveMode), [this](sys::DataMessage *, sys::ResponseMessage *) {
            handlePowerSavingModeInit();
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(APMPreventBlocking), [this](sys::DataMessage *, sys::ResponseMessage *) {
            blockingTimer->reload();
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(APMSwitch), [this](sys::DataMessage *request, sys::ResponseMessage *) {
            auto msg = static_cast<APMSwitch *>(request);
            handleSwitchApplication(msg);
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(APMSwitchPrevApp), [this](sys::DataMessage *request, sys::ResponseMessage *) {
            auto msg = static_cast<APMSwitchPrevApp *>(request);
            handleSwitchBack(msg);
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(APMConfirmSwitch), [this](sys::DataMessage *request, sys::ResponseMessage *) {
            auto msg = static_cast<APMConfirmSwitch *>(request);
            handleSwitchConfirmation(msg);
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(APMConfirmClose), [this](sys::DataMessage *request, sys::ResponseMessage *) {
            auto msg = static_cast<APMConfirmClose *>(request);
            handleCloseConfirmation(msg);
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(APMDelayedClose), [this](sys::DataMessage *request, sys::ResponseMessage *) {
            auto msg = static_cast<APMDelayedClose *>(request);
            closeService(msg->getApplication());
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(APMRegister), [this](sys::DataMessage *request, sys::ResponseMessage *) {
            auto msg = static_cast<APMRegister *>(request);
            handleRegisterApplication(msg);
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(APMChangeLanguage), [this](sys::DataMessage *request, sys::ResponseMessage *) {
            auto msg = static_cast<APMChangeLanguage *>(request);
            handleLanguageChange(msg);
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(APMClose), [this](sys::DataMessage *, sys::ResponseMessage *) {
            closeApplications();
            closeServices();
            return std::make_shared<sys::ResponseMessage>();
        });
        connect(typeid(APMAction), [this](sys::DataMessage *request, sys::ResponseMessage *) {
            auto actionMsg = static_cast<APMAction *>(request);
            handleAction(actionMsg);
            return std::make_shared<sys::ResponseMessage>();
        });
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
                closeService(app->name());
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

    auto ApplicationManager::handlePowerSavingModeInit() -> bool
    {
        LOG_INFO("Going to suspend mode");
        suspendSystemServices();
        sys::SystemManager::SuspendSystem(this);
        return true;
    }

    auto ApplicationManager::handleSwitchApplication(APMSwitch *msg) -> bool
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
        const bool isFocusedAppCloseable = !(app->switchData && app->switchData->disableAppClose) &&
                                           currentlyFocusedApp->closeable() && !currentlyFocusedApp->blockClosing;
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

    auto ApplicationManager::handleAction(APMAction *actionMsg) -> bool
    {
        auto &action         = actionMsg->getAction();
        const auto targetApp = getApplication(action.targetApplication);
        if (targetApp == nullptr) {
            LOG_ERROR("Failed to switch to %s application: No such application.", action.targetApplication.c_str());
            return false;
        }

        if (targetApp->state() == app::Application::State::ACTIVE_FORGROUND) {
            // If the app is already focused, then switch window.
            auto msg = std::make_shared<app::AppSwitchWindowMessage>(
                action.targetWindow, std::string{}, std::move(action.data), gui::ShowMode::GUI_SHOW_INIT);
            return sys::Bus::SendUnicast(msg, targetApp->name(), this);
        }
        APMSwitch switchRequest(
            actionMsg->getSenderName(), targetApp->name(), action.targetWindow, std::move(action.data));
        return handleSwitchApplication(&switchRequest);
    }

    auto ApplicationManager::handleSwitchBack(APMSwitchPrevApp *msg) -> bool
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

        LOG_DEBUG("Switch applications: [%s](%s) -> [%s](%s)",
                  currentlyFocusedApp->name().c_str(),
                  app::Application::stateStr(currentlyFocusedApp->state()),
                  previousApp->name().c_str(),
                  app::Application::stateStr(previousApp->state()));

        onApplicationSwitchToPrev(*previousApp, std::move(msg->getData()));
        requestApplicationClose(*currentlyFocusedApp, currentlyFocusedApp->closeable());
        return true;
    }

    void ApplicationManager::onApplicationSwitchToPrev(ApplicationHandle &previousApp,
                                                       std::unique_ptr<gui::SwitchData> &&data,
                                                       std::string targetWindow)
    {
        popApplication();
        previousApp.switchData   = std::move(data);
        previousApp.switchWindow = std::move(targetWindow);
    }

    auto ApplicationManager::handleRegisterApplication(APMRegister *msg) -> bool
    {
        auto app = getApplication(msg->getSenderName());
        if (app == nullptr) {
            LOG_ERROR("Failed to register %s: No such application.", msg->getSenderName().c_str());
            return false;
        }

        if (msg->getStatus()) {
            onApplicationRegistered(*app, msg->getStartBackground());
        }
        else {
            onApplicationRegistrationFailure(*app);
        }

        auto notification = std::make_shared<APMCheckApp>(GetName(), app->name());
        sys::Bus::SendMulticast(notification, sys::BusChannels::AppManagerNotifications, this);
        return true;
    }

    void ApplicationManager::onApplicationRegistered(ApplicationHandle &app, bool startInBackground)
    {
        LOG_DEBUG("Application %s registered successfully.", app.name().c_str());

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

    void ApplicationManager::onApplicationRegistrationFailure(ApplicationHandle &app)
    {
        LOG_ERROR("Failed to register %s: Application initialisation error.", app.name().c_str());
        Controller::switchBack(this);
    }

    auto ApplicationManager::handleLanguageChange(app::manager::APMChangeLanguage *msg) -> bool
    {
        const auto requestedLanguage = toSettingsLanguage(msg->getLanguage());
        if (requestedLanguage == settings.language) {
            LOG_WARN("The selected language is already set. Ignore.");
            return true;
        }

        settings          = DBServiceAPI::SettingsGet(this);
        settings.language = requestedLanguage;

        DBServiceAPI::SettingsUpdate(this, settings);
        utils::localize.Switch(msg->getLanguage());
        rebuildActiveApplications();
        return true;
    }

    void ApplicationManager::rebuildActiveApplications()
    {
        for (const auto &app : getApplications()) {
            if (app && app->launcher && app->launcher->handle) {
                if (const auto appState = app->state(); appState == ApplicationHandle::State::ACTIVE_FORGROUND ||
                                                        appState == ApplicationHandle::State::ACTIVE_BACKGROUND) {
                    app::Application::messageRebuildApplication(this, app->name());
                }
            }
        }
    }

    auto ApplicationManager::handleSwitchConfirmation(APMConfirmSwitch *msg) -> bool
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

    auto ApplicationManager::handleCloseConfirmation(APMConfirmClose *msg) -> bool
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
} // namespace app::manager
