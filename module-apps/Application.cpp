// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Application.hpp"
#include "Common.hpp"      // for RefreshModes
#include "GuiTimer.hpp"    // for GuiTimer
#include "Item.hpp"        // for Item
#include "MessageType.hpp" // for MessageType
#include "module-apps/popups/data/PopupRequestParams.hpp"
#include "module-apps/popups/data/PhoneModeParams.hpp"
#include "module-sys/Timers/TimerFactory.hpp" // for Timer
#include "TopBar.hpp"
#include "TopBar/Time.hpp"
#include "popups/TetheringConfirmationPopup.hpp"
#include "Translator.hpp"                // for KeyInputSim...
#include "common_data/EventStore.hpp"    // for Battery
#include "common_data/RawKey.hpp"        // for RawKey, key...
#include "gui/input/InputEvent.hpp"      // for InputEvent
#include "log/debug.hpp"                 // for DEBUG_APPLI...
#include "log/log.hpp"                   // for LOG_INFO
#include "messages/AppMessage.hpp"       // for AppSwitchMe...
#include "service-appmgr/Controller.hpp" // for Controller
#include <service-cellular/CellularMessage.hpp>
#include <service-evtmgr/BatteryMessages.hpp>
#include <service-evtmgr/Constants.hpp>
#include <service-evtmgr/EVMessages.hpp>
#include <service-appmgr/service-appmgr/messages/DOMRequest.hpp>
#include <service-appmgr/messages/UserPowerDownRequest.hpp>
#include <service-appmgr/data/NotificationsChangedActionsParams.hpp>
#include "service-gui/messages/DrawMessage.hpp" // for DrawMessage
#include "task.h"                               // for xTaskGetTic...
#include "windows/AppWindow.hpp"                // for AppWindow
#include "DOMResponder.hpp"
#include <Text.hpp>    // for Text
#include <algorithm>   // for find
#include <iterator>    // for distance, next
#include <type_traits> // for add_const<>...
#include <WindowsFactory.hpp>
#include <service-gui/Common.hpp>
#include <module-utils/Utils.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <module-utils/time/DateAndTimeSettings.hpp>

#include <service-audio/AudioServiceAPI.hpp> // for GetOutputVolume
#include <TetheringPhoneModePopup.hpp>
#include "popups/data/PopupData.hpp"

namespace gui
{
    class DrawCommand;
}

namespace app
{
    auto actionHandled() -> ActionResult
    {
        return std::make_shared<ActionHandledResponse>();
    }

    auto actionNotHandled() -> ActionResult
    {
        return std::make_shared<ActionHandledResponse>(sys::ReturnCodes::Failure);
    }

    const char *Application::stateStr(Application::State st)
    {
        switch (st) {
        case State::NONE:
            return "NONE";
        case State::DEACTIVATED:
            return "DEACTIVATED";
        case State::INITIALIZING:
            return "INITIALIZING";
        case State::ACTIVATING:
            return "ACTIVATING";
        case State::ACTIVE_FORGROUND:
            return "ACTIVE_FORGROUND";
        case State::ACTIVE_BACKGROUND:
            return "ACTIVE_BACKGROUND";
        case State::DEACTIVATING:
            return "DEACTIVATING";
        default:
            return "FixIt";
        }
    }

    Application::Application(std::string name,
                             std::string parent,
                             sys::phone_modes::PhoneMode mode,
                             StartInBackground startInBackground,
                             uint32_t stackDepth,
                             sys::ServicePriority priority)
        : Service(std::move(name), std::move(parent), stackDepth, priority),
          default_window(gui::name::window::main_window), windowsStack(this),
          keyTranslator{std::make_unique<gui::KeyInputSimpleTranslation>()}, startInBackground{startInBackground},
          callbackStorage{std::make_unique<CallbackStorage>()}, topBarManager{std::make_unique<TopBarManager>()},
          settings(std::make_unique<settings::Settings>(this)), phoneMode{mode}
    {
        topBarManager->enableIndicators({gui::top_bar::Indicator::Time});
        using TimeMode = gui::top_bar::TimeConfiguration::TimeMode;
        auto modifier  = std::make_shared<gui::top_bar::TimeConfiguration>(
            utils::dateAndTimeSettings.isTimeFormat12() ? TimeMode::Time12h : TimeMode::Time24h);
        topBarManager->set(gui::top_bar::Indicator::Time, std::move(modifier));

        bus.channels.push_back(sys::BusChannel::ServiceCellularNotifications);

        longPressTimer = sys::TimerFactory::createPeriodicTimer(this,
                                                                "LongPress",
                                                                std::chrono::milliseconds{key_timer_ms},
                                                                [this](sys::Timer &) { longPressTimerCallback(); });

        connect(typeid(AppRefreshMessage),
                [this](sys::Message *msg) -> sys::MessagePointer { return handleAppRefresh(msg); });
        connect(sevm::BatteryStatusChangeMessage(), [&](sys::Message *) { return handleBatteryStatusChange(); });
        connect(typeid(app::manager::DOMRequest),
                [&](sys::Message *msg) -> sys::MessagePointer { return handleGetDOM(msg); });
        connect(typeid(AppUpdateWindowMessage),
                [&](sys::Message *msg) -> sys::MessagePointer { return handleUpdateWindow(msg); });

        addActionReceiver(app::manager::actions::PhoneModeChanged, [this](auto &&params) {
            if (params != nullptr) {
                auto modeParams = static_cast<gui::PhoneModeParams *>(params.get());
                phoneMode       = modeParams->getPhoneMode();
            }
            return actionHandled();
        });
        addActionReceiver(app::manager::actions::ShowPopup, [this](auto &&params) {
            auto popupParams = static_cast<gui::PopupRequestParams *>(params.get());
            if (const auto popupId = popupParams->getPopupId(); isPopupPermitted(popupId)) {
                showPopup(popupId, popupParams);
            }
            return actionHandled();
        });
        addActionReceiver(app::manager::actions::AbortPopup, [this](auto &&params) {
            auto popupParams   = static_cast<gui::PopupRequestParams *>(params.get());
            const auto popupId = popupParams->getPopupId();
            abortPopup(popupId);
            return actionHandled();
        });
        addActionReceiver(app::manager::actions::NotificationsChanged, [this](auto &&params) {
            auto notificationParams = static_cast<manager::actions::NotificationsChangedParams *>(params.get());
            handle(notificationParams);
            return actionHandled();
        });
    }

    Application::~Application() noexcept
    {
        windowsStack.windows.clear();
        settings->unregisterValueChange();
    }

    Application::State Application::getState()
    {
        return state;
    }

    void Application::setState(State st)
    {
#if DEBUG_APPLICATION_MANAGEMENT == 1
        LOG_DEBUG("[%s] (%s) -> (%s)", GetName().c_str(), stateStr(state), stateStr(st));
#endif
        state = st;
    }

    void Application::longPressTimerCallback()
    {
        // TODO if(check widget type long press trigger)
        uint32_t time = xTaskGetTickCount();
        if (keyTranslator->timeout(time)) {
            // previous key press was over standard keypress timeout - send long press
            gui::InputEvent iev = keyTranslator->translate(time);
            messageInputEventApplication(this, this->GetName(), iev);
            // clean previous key
            keyTranslator->prev_key_press = {};
            longPressTimer.stop();
        }
    }

    void Application::clearLongPressTimeout()
    {
        keyTranslator->prev_key_timedout = false;
    }

    void Application::render(gui::RefreshModes mode)
    {
        if (windowsStack.isEmpty()) {
            LOG_ERROR("Current window is not defined");
            return;
        }

        // send drawing commands only when if application is in active and visible.
        if (state == State::ACTIVE_FORGROUND) {
            auto window = getCurrentWindow();
            window->updateBatteryStatus();
            window->updateSim();
            window->updateSignalStrength();
            window->updateNetworkAccessTechnology();
            window->updateTime();
            window->updatePhoneMode(phoneMode);

            auto message = std::make_shared<service::gui::DrawMessage>(window->buildDrawList(), mode);

            if (systemCloseInProgress) {
                message->setCommandType(service::gui::DrawMessage::Type::SHUTDOWN);
            }
            else if (suspendInProgress) {
                message->setCommandType(service::gui::DrawMessage::Type::SUSPEND);
            }

            bus.sendUnicast(std::move(message), service::name::gui);
        }

        if (suspendInProgress)
            suspendInProgress = false;
    }

    void Application::updateWindow(const std::string &windowName, std::unique_ptr<gui::SwitchData> data)
    {
        const auto currentWindow = getCurrentWindow();
        auto msg =
            std::make_shared<AppUpdateWindowMessage>(currentWindow ? currentWindow->getName() : "", std::move(data));
        bus.sendUnicast(std::move(msg), this->GetName());
    }

    void Application::switchWindow(const std::string &windowName,
                                   gui::ShowMode cmd,
                                   std::unique_ptr<gui::SwitchData> data)
    {

        std::string window;
#if DEBUG_APPLICATION_MANAGEMENT == 1
        LOG_INFO("switching [%s] to window: %s data description: %s",
                 GetName().c_str(),
                 windowName.length() ? windowName.c_str() : default_window.c_str(),
                 data ? data->getDescription().c_str() : "");
#endif

        // case to handle returning to previous application
        if (windowName.empty()) {
            window = getCurrentWindow()->getName();
            auto msg =
                std::make_shared<AppSwitchWindowMessage>(window, getCurrentWindow()->getName(), std::move(data), cmd);
            bus.sendUnicast(msg, this->GetName());
        }
        else {
            auto msg = std::make_shared<AppSwitchWindowMessage>(
                windowName, getCurrentWindow() ? getCurrentWindow()->getName() : "", std::move(data), cmd);
            bus.sendUnicast(msg, this->GetName());
        }
    }

    void Application::returnToPreviousWindow(const uint32_t times)
    {
        if (const auto prevWindow = getPrevWindow(times); prevWindow == gui::name::window::no_window) {
            app::manager::Controller::switchBack(this);
        }
        else {
            LOG_INFO("Back to previous window %s", prevWindow.c_str());
            switchWindow(prevWindow, gui::ShowMode::GUI_SHOW_RETURN);
        }
    }

    void Application::refreshWindow(gui::RefreshModes mode)
    {
        if (not windowsStack.isEmpty()) {
            auto msg = std::make_shared<AppRefreshMessage>(mode, getCurrentWindow()->getName());
            bus.sendUnicast(msg, this->GetName());
        }
    }

    sys::MessagePointer Application::DataReceivedHandler(sys::DataMessage *msgl)
    {
        auto msg = dynamic_cast<CellularNotificationMessage *>(msgl);
        if (msg != nullptr) {
            if (msg->content == CellularNotificationMessage::Content::SignalStrengthUpdate) {
                return handleSignalStrengthUpdate(msgl);
            }
            if (msg->content == CellularNotificationMessage::Content::NetworkStatusUpdate) {
                return handleNetworkAccessTechnologyUpdate(msgl);
            }
        }
        else if (msgl->messageType == MessageType::AppInputEvent) {
            return handleInputEvent(msgl);
        }
        else if (msgl->messageType == MessageType::KBDKeyEvent) {
            return handleKBDKeyEvent(msgl);
        }
        else if (msgl->messageType == MessageType::EVMMinuteUpdated) {
            return handleMinuteUpdated(msgl);
        }
        else if (msgl->messageType == MessageType::AppAction) {
            return handleAction(msgl);
        }
        else if (msgl->messageType == MessageType::AppSwitch) {
            return handleApplicationSwitch(msgl);
        }
        else if (msgl->messageType == MessageType::AppSwitchWindow) {
            return handleSwitchWindow(msgl);
        }
        else if (msgl->messageType == MessageType::AppClose) {
            return handleAppClose(msgl);
        }
        else if (msgl->messageType == MessageType::AppRebuild) {
            return handleAppRebuild(msg);
        }
        else if (msgl->messageType == MessageType::AppFocusLost) {
            return handleAppFocusLost(msgl);
        }
        else if (dynamic_cast<sevm::SIMMessage *>(msgl) != nullptr) {
            return handleSIMMessage(msgl);
        }
        return sys::msgNotHandled();
    }

    sys::MessagePointer Application::handleSignalStrengthUpdate(sys::Message *msgl)
    {
        if ((state == State::ACTIVE_FORGROUND) && getCurrentWindow()->updateSignalStrength()) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        return sys::msgHandled();
    }

    sys::MessagePointer Application::handleNetworkAccessTechnologyUpdate(sys::Message *msgl)
    {
        if ((state == State::ACTIVE_FORGROUND) && getCurrentWindow()->updateNetworkAccessTechnology()) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        return sys::msgHandled();
    }

    sys::MessagePointer Application::handleInputEvent(sys::Message *msgl)
    {
        AppInputEventMessage *msg = reinterpret_cast<AppInputEventMessage *>(msgl);
        if (msg->getEvent().state == gui::InputEvent::State::keyPressed) {
            longPressTimer.start();
        }
        else if (msg->getEvent().state == gui::InputEvent::State::keyReleasedShort) {
            longPressTimer.stop();
        }
        if (not windowsStack.isEmpty() && getCurrentWindow()->onInput(msg->getEvent())) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        return sys::msgHandled();
    }

    sys::MessagePointer Application::handleKBDKeyEvent(sys::Message *msgl)
    {
        if (this->getState() != app::Application::State::ACTIVE_FORGROUND) {
            LOG_FATAL("!!! Terrible terrible damage! application with no focus grabbed key!");
        }
        sevm::KbdMessage *msg = static_cast<sevm::KbdMessage *>(msgl);
        gui::InputEvent iev   = keyTranslator->translate(msg->key);
        if (iev.keyCode != gui::KeyCode::KEY_UNDEFINED) {
            messageInputEventApplication(this, this->GetName(), iev);
        }
        return sys::msgHandled();
    }

    sys::MessagePointer Application::handleBatteryStatusChange()
    {
        if ((state == State::ACTIVE_FORGROUND) && getCurrentWindow()->updateBatteryStatus()) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        return sys::msgHandled();
    }

    sys::MessagePointer Application::handleMinuteUpdated(sys::Message *msgl)
    {
        if (state == State::ACTIVE_FORGROUND && getCurrentWindow()->updateTime()) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        return sys::msgHandled();
    }

    sys::MessagePointer Application::handleAction(sys::Message *msgl)
    {
        auto *msg         = static_cast<AppActionRequest *>(msgl);
        const auto action = msg->getAction();
        try {
            const auto &actionHandler = receivers.at(action);
            auto &data                = msg->getData();

            auto result = actionHandler(std::move(data));

            if (windowsStack.isEmpty()) {
                LOG_ERROR("OnAction application switch with no window provided. Fallback to default mainWindow.");
                setActiveWindow(gui::name::window::main_window);
            }

            return result;
        }
        catch (const std::out_of_range &) {
            LOG_ERROR("Application %s is not able to handle action #%d", GetName().c_str(), action);
        }
        return actionNotHandled();
    }

    sys::MessagePointer Application::handleApplicationSwitch(sys::Message *msgl)
    {
        auto *msg = static_cast<AppSwitchMessage *>(msgl);

        switch (msg->getApplicationStartupReason()) {
        case StartupReason::Launch:
            return handleApplicationSwitchLaunch(msgl);
        case StartupReason::OnAction:
            return handleApplicationSwitchOnAction(msgl);
        }

        return sys::msgNotHandled();
    }

    sys::MessagePointer Application::handleApplicationSwitchLaunch(sys::Message *msgl)
    {
        auto *msg    = static_cast<AppSwitchMessage *>(msgl);
        bool handled = false;
        LOG_DEBUG("AppSwitch: %s", msg->getTargetApplicationName().c_str());
        // Application is starting or it is in the background. Upon switch command if name if correct it goes
        // foreground
        if ((state == State::ACTIVATING) || (state == State::INITIALIZING) || (state == State::ACTIVE_BACKGROUND)) {

            if (msg->getTargetApplicationName() == this->GetName()) {
                setState(State::ACTIVE_FORGROUND);
                if (app::manager::Controller::confirmSwitch(this)) {
                    LOG_INFO("target Window: %s : target description: %s",
                             msg->getTargetWindowName().c_str(),
                             msg->getData() ? msg->getData()->getDescription().c_str() : "");
                    switchWindow(msg->getTargetWindowName(), std::move(msg->getData()));
                    handled = true;
                }
                else {
                    // TODO send to itself message to close
                    LOG_ERROR("Failed to communicate ");
                }
            }
            else {
                LOG_ERROR("Received switch message outside of activation flow");
            }
        }
        else if (state == State::ACTIVE_FORGROUND) {
            if (msg->getTargetApplicationName() == GetName()) {
                switchWindow(msg->getTargetWindowName(), std::move(msg->getData()));
                handled = true;
            }
            else {
                LOG_ERROR("Received switch message outside of activation flow");
            }
        }
        else {
            LOG_ERROR("Wrong internal application %s switch to ACTIVE state form %s",
                      msg->getTargetApplicationName().c_str(),
                      stateStr(state));
        }
        if (handled) {
            return sys::msgHandled();
        }
        return sys::msgNotHandled();
    }

    sys::MessagePointer Application::handleApplicationSwitchOnAction(sys::Message *msgl)
    {
        if ((state == State::ACTIVATING) || (state == State::INITIALIZING) || (state == State::ACTIVE_BACKGROUND)) {
            setState(State::ACTIVE_FORGROUND);
            app::manager::Controller::confirmSwitch(this);
            return sys::msgHandled();
        }
        else {
            LOG_ERROR("Application already running - no startup on Action");
            return sys::msgNotHandled();
        }
    }

    sys::MessagePointer Application::handleSwitchWindow(sys::Message *msgl)
    {
        auto msg = static_cast<AppSwitchWindowMessage *>(msgl);
        // check if specified window is in the application

        if (windowsFactory.isRegistered(msg->getWindowName())) {
            auto switchData = std::move(msg->getData());
            if (switchData && switchData->ignoreCurrentWindowOnStack) {
                popToWindow(getPrevWindow());
            }
            if (!isCurrentWindow(msg->getWindowName())) {
                if (!windowsStack.isEmpty()) {
                    getCurrentWindow()->onClose();
                }
                setActiveWindow(msg->getWindowName());
            }
            LOG_DEBUG("Current window: %s vs %s", getCurrentWindow()->getName().c_str(), msg->getWindowName().c_str());
            getCurrentWindow()->handleSwitchData(switchData.get());

            auto ret = dynamic_cast<gui::SwitchSpecialChar *>(switchData.get());
            if (ret != nullptr && switchData != nullptr) {
                auto text = dynamic_cast<gui::Text *>(getCurrentWindow()->getFocusItem());
                if (text != nullptr) {
                    text->addText(ret->getDescription());
                    refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                    return sys::msgHandled();
                }
            }
            getCurrentWindow()->onBeforeShow(msg->getCommand(), switchData.get());
            refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
        }
        else {
            LOG_ERROR("No such window: %s", msg->getWindowName().c_str());
        }
        return sys::msgHandled();
    }

    sys::MessagePointer Application::handleUpdateWindow(sys::Message *msgl)
    {
        auto msg = static_cast<AppUpdateWindowMessage *>(msgl);

        if (windowsFactory.isRegistered(msg->getWindowName()) && isCurrentWindow(msg->getWindowName())) {
            const auto &switchData = msg->getData();
            getCurrentWindow()->handleSwitchData(switchData.get());
            getCurrentWindow()->onBeforeShow(msg->getCommand(), switchData.get());
            refreshWindow(msg->getRefreshMode());
        }
        else {
            LOG_ERROR("No such window: %s", msg->getWindowName().c_str());
        }
        return sys::msgHandled();
    }

    sys::MessagePointer Application::handleAppClose(sys::Message *msgl)
    {
        setState(State::DEACTIVATING);
        app::manager::Controller::confirmClose(this);
        return sys::msgHandled();
    }

    sys::MessagePointer Application::handleAppRebuild(sys::Message *msgl)
    {
        LOG_INFO("Application %s rebuilding gui", GetName().c_str());
        for (auto &[name, window] : windowsStack) {
            LOG_DEBUG("Rebuild: %s", name.c_str());
            windowsStack.windows[name] = windowsFactory.build(this, name);
        }
        LOG_INFO("Refresh app with focus!");
        if (state == State::ACTIVE_FORGROUND) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
        }
        LOG_INFO("App rebuild done");
        return sys::msgHandled();
    }

    sys::MessagePointer Application::handleAppRefresh(sys::Message *msgl)
    {
        auto *msg = static_cast<AppRefreshMessage *>(msgl);
        assert(msg);
        if (windowsStack.isEmpty() || (getCurrentWindow()->getName() != msg->getWindowName())) {
            LOG_DEBUG("Ignore request for window %s we are on window %s",
                      msg->getWindowName().c_str(),
                      windowsStack.isEmpty() ? "none" : getCurrentWindow()->getName().c_str());
            return sys::msgNotHandled();
        }
        render(msg->getMode());
        return sys::msgHandled();
    }

    sys::MessagePointer Application::handleGetDOM(sys::Message *msgl)
    {
        if (windowsStack.isEmpty()) {
            LOG_ERROR("Current window is not defined - can't dump DOM");
            return sys::msgNotHandled();
        }
        auto window = getCurrentWindow();
        if (window == nullptr) {
            LOG_ERROR("No window - can't dump DOM");
            return sys::msgNotHandled();
        }

        auto request = static_cast<app::manager::DOMRequest *>(msgl);
        LOG_DEBUG("Get DOM for: %s", request->getSenderName().c_str());
        bus.sendUnicast(DOMResponder(GetName(), *window, std::move(request->event)).build(), request->getSenderName());

        return sys::msgHandled();
    }

    sys::MessagePointer Application::handleAppFocusLost(sys::Message *msgl)
    {
        if (state == State::ACTIVE_FORGROUND) {
            setState(State::ACTIVE_BACKGROUND);
            app::manager::Controller::confirmSwitch(this);
        }
        return sys::msgHandled();
    }

    sys::MessagePointer Application::handleSIMMessage(sys::Message *msgl)
    {
        if (getCurrentWindow()->updateSim()) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        return sys::msgHandled();
    }

    sys::ReturnCodes Application::InitHandler()
    {
        setState(State::INITIALIZING);

        app::manager::Controller::applicationInitialised(this, StartupStatus::Success, startInBackground);

        if (startInBackground) {
            setState(State::ACTIVE_BACKGROUND);
        }

        lockScreenPasscodeIsOn = utils::getNumericValue<bool>(
            settings->getValue(settings::SystemProperties::lockScreenPasscodeIsOn, settings::SettingsScope::Global));
        settings->registerValueChange(
            settings::SystemProperties::lockScreenPasscodeIsOn,
            [this](const std::string &value) { lockScreenPasscodeIsOn = utils::getNumericValue<bool>(value); },
            settings::SettingsScope::Global);
        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes Application::DeinitHandler()
    {
        LOG_INFO("Closing an application: %s", GetName().c_str());

        for (const auto &[windowName, window] : windowsStack) {
            LOG_INFO("Closing a window: %s", windowName.c_str());
            window->onClose();
        }

        windowsStack.windows.clear();
        return sys::ReturnCodes::Success;
    }

    void Application::setActiveWindow(const std::string &windowName)
    {
        pushWindow(windowName);
    }

    bool Application::adjustCurrentVolume(const int step)
    {
        return AudioServiceAPI::KeyPressed(this, step);
    }

    void Application::toggleTorchOnOff()
    {
        auto msg = std::make_shared<sevm::ToggleTorchOnOffMessage>();
        bus.sendUnicast(std::move(msg), service::name::evt_manager);
    }

    void Application::toggleTorchColor()
    {
        auto msg = std::make_shared<sevm::ToggleTorchColorMessage>();
        bus.sendUnicast(std::move(msg), service::name::evt_manager);
    }

    void Application::requestAction(sys::Service *sender,
                                    const ApplicationName &applicationName,
                                    manager::actions::ActionId actionId,
                                    manager::actions::ActionParamsPtr &&data)
    {
        auto msg = std::make_shared<AppActionRequest>(actionId, std::move(data));
        sender->bus.sendUnicast(msg, applicationName);
    }

    void Application::messageSwitchApplication(sys::Service *sender,
                                               std::string application,
                                               std::string window,
                                               std::unique_ptr<gui::SwitchData> data,
                                               StartupReason startupReason)
    {
        auto msg = std::make_shared<AppSwitchMessage>(application, window, std::move(data), startupReason);
        sender->bus.sendUnicast(msg, application);
    }

    void Application::messageCloseApplication(sys::Service *sender, std::string application)
    {
        auto msg = std::make_shared<AppMessage>(MessageType::AppClose);
        sender->bus.sendUnicast(msg, application);
    }

    void Application::messageRebuildApplication(sys::Service *sender, std::string application)
    {
        auto msg = std::make_shared<AppRebuildMessage>();
        sender->bus.sendUnicast(msg, application);
    }

    void Application::messageApplicationLostFocus(sys::Service *sender, std::string application)
    {
        auto msg = std::make_shared<AppLostFocusMessage>();
        sender->bus.sendUnicast(msg, application);
    }

    void Application::messageInputEventApplication(sys::Service *sender,
                                                   std::string application,
                                                   const gui::InputEvent &event)
    {
        auto msg = std::make_shared<AppInputEventMessage>(event);
        sender->bus.sendUnicast(msg, application);
    }

    void Application::handlePhoneModeChanged(sys::phone_modes::PhoneMode mode)
    {
        auto flightModeSetting = settings->getValue(settings::Cellular::offlineMode, settings::SettingsScope::Global);
        bool flightMode        = flightModeSetting == "1" ? true : false;

        using namespace gui::popup;
        const auto &popupName = resolveWindowName(gui::popup::ID::PhoneModes);
        if (const auto currentWindowName = getCurrentWindow()->getName(); currentWindowName == popupName) {
            updateWindow(popupName, std::make_unique<gui::ModesPopupData>(mode, flightMode));
        }
        else {
            switchWindow(popupName, std::make_unique<gui::ModesPopupData>(mode, flightMode));
        }
    }

    void Application::handleVolumeChanged(audio::Volume volume, audio::Context context)
    {
        using namespace gui::popup;
        const auto popupName = resolveWindowName(gui::popup::ID::Volume);
        if (const auto currentWindowName = getCurrentWindow()->getName(); currentWindowName == popupName) {
            updateWindow(popupName, std::make_unique<gui::VolumePopupData>(volume, context));
        }
        else {
            switchWindow(popupName, std::make_unique<gui::VolumePopupData>(volume, context));
        }
    }

    void Application::attachPopups(const std::vector<gui::popup::ID> &popupsList)
    {
        using namespace gui::popup;
        for (auto popup : popupsList) {
            switch (popup) {
            case ID::Volume:
                windowsFactory.attach(window::volume_window, [](Application *app, const std::string &name) {
                    return std::make_unique<gui::VolumeWindow>(app, window::volume_window);
                });
                break;
            case ID::Tethering:
                windowsFactory.attach(window::tethering_confirmation_window,
                                      [](Application *app, const std::string &name) {
                                          return std::make_unique<gui::TetheringConfirmationPopup>(
                                              app, window::tethering_confirmation_window);
                                      });
                break;
            case ID::TetheringPhoneModeChangeProhibited:
                windowsFactory.attach(window::tethering_phonemode_change_window,
                                      [](Application *app, const std::string &name) {
                                          return std::make_unique<gui::TetheringPhoneModePopup>(
                                              app, window::tethering_phonemode_change_window);
                                      });
                break;
            case ID::PhoneModes:
                windowsFactory.attach(window::phone_modes_window, [](Application *app, const std::string &name) {
                    return std::make_unique<gui::HomeModesWindow>(app, window::phone_modes_window);
                });
                break;
            case ID::Brightness:
                break;
            }
        }
    }

    void Application::showPopup(gui::popup::ID id, const gui::PopupRequestParams *params)
    {
        using namespace gui::popup;
        if (id == ID::PhoneModes) {
            auto popupParams = static_cast<const gui::PhoneModePopupRequestParams *>(params);
            handlePhoneModeChanged(popupParams->getPhoneMode());
        }
        else if (id == ID::Volume) {
            auto volumeParams = static_cast<const gui::VolumePopupRequestParams *>(params);
            LOG_INFO("Playback: %s, volume: %s",
                     audio::str(volumeParams->getAudioContext().second).c_str(),
                     std::to_string(volumeParams->getVolume()).c_str());
            handleVolumeChanged(volumeParams->getVolume(), volumeParams->getAudioContext());
        }
        else {
            switchWindow(gui::popup::resolveWindowName(id));
        }
    }

    void Application::abortPopup(gui::popup::ID id)
    {
        const auto popupName = gui::popup::resolveWindowName(id);
        if (getCurrentWindow()->getName() == popupName) {
            returnToPreviousWindow();
        }
    }

    bool Application::isPopupPermitted([[maybe_unused]] gui::popup::ID popupId) const
    {
        return true;
    }

    bool Application::popToWindow(const std::string &window)
    {
        if (window == gui::name::window::no_window) {
            bool ret = false;
            if (windowsStack.stack.size() <= 1) {
                windowsStack.stack.clear();
                ret = true;
            }
            return ret;
        }

        auto ret = std::find(windowsStack.stack.begin(), windowsStack.stack.end(), window);
        if (ret != windowsStack.stack.end()) {
            LOG_INFO("Pop last window(s) [%d] :  %s",
                     static_cast<int>(std::distance(ret, windowsStack.stack.end())),
                     ret->c_str());
            windowsStack.stack.erase(std::next(ret), windowsStack.stack.end());
            LOG_INFO("Curent window... %s vs %s", ret->c_str(), windowsStack.stack.back().c_str());
            return true;
        }
        return false;
    }

    void Application::pushWindow(const std::string &newWindow)
    {
        // handle if window was already on
        LOG_DEBUG("App: %s window %s request", GetName().c_str(), newWindow.c_str());
        if (popToWindow(newWindow)) {
            return;
        }
        else {
            windowsStack.push(newWindow, windowsFactory.build(this, newWindow));
        }
#if DEBUG_APPLICATION_MANAGEMENT == 1
        LOG_DEBUG("[%d] newWindow: %s", (int)windowsStack.stack.size(), newWindow.c_str());
        for (auto &el : windowsStack.stack) {
            LOG_DEBUG("-> %s", el.c_str());
        }
#endif
    };

    const std::string Application::getPrevWindow(uint32_t count) const
    {
        if (this->windowsStack.stack.size() <= 1 || count > this->windowsStack.stack.size()) {
            return gui::name::window::no_window;
        }
        return *std::prev(windowsStack.stack.end(), count + 1);
    }

    gui::AppWindow *Application::getCurrentWindow()
    {
        if (windowsStack.stack.size() == 0) {
            windowsStack.push(default_window, windowsFactory.build(this, default_window));
        }
        /// TODO handle nullptr? if not found on stack -> return default
        return windowsStack.get(windowsStack.stack.back());
    }

    bool Application::isCurrentWindow(const std::string &windowName) const noexcept
    {
        if (windowsStack.isEmpty()) {
            return false;
        }
        return windowsStack.stack.back() == windowName;
    }

    gui::AppWindow *Application::getWindow(const std::string &name)
    {
        return windowsStack.get(name);
    }

    void Application::connect(GuiTimer *timer, gui::Item *item)
    {
        item->attachTimer(timer);
    }

    const gui::top_bar::Configuration &Application::getTopBarConfiguration() const noexcept
    {
        return topBarManager->getConfiguration();
    }

    void Application::addActionReceiver(manager::actions::ActionId actionId, OnActionReceived &&callback)
    {
        receivers.insert_or_assign(actionId, std::move(callback));
    }

    void Application::handle(manager::actions::NotificationsChangedParams *params)
    {
        LOG_DEBUG("To be implemented by Pop-up based Locked-Screen [EGD-5884]");
    }

    void Application::cancelCallbacks(AsyncCallbackReceiver::Ptr receiver)
    {
        callbackStorage->removeAll(receiver);
    }

    void Application::setLockScreenPasscodeOn(bool screenPasscodeOn) noexcept
    {
        lockScreenPasscodeIsOn = screenPasscodeOn;
        settings->setValue(settings::SystemProperties::lockScreenPasscodeIsOn,
                           std::to_string(lockScreenPasscodeIsOn),
                           settings::SettingsScope::Global);
    }

    bool Application::isLockScreenPasscodeOn() const noexcept
    {
        return lockScreenPasscodeIsOn;
    }
} /* namespace app */
