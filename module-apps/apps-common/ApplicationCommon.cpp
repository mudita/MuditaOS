﻿// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationCommon.hpp"
#include "GuiTimer.hpp"            // for GuiTimer
#include "Timers/TimerFactory.hpp" // for Timer
#include "status-bar/Time.hpp"
#include "messages/AppSwitchWindowPopupMessage.hpp"
#include "service-appmgr/Controller.hpp" // for Controller
#include "actions/AlarmClockStatusChangeParams.hpp"
#include <service-cellular/CellularMessage.hpp>
#include <service-desktop/DesktopMessages.hpp>
#include <service-evtmgr/ServiceEventManagerName.hpp>
#include <service-evtmgr/EVMessages.hpp>
#include <service-appmgr/messages/DOMRequest.hpp>
#include <service-appmgr/data/NotificationsChangedActionsParams.hpp>
#include "service-gui/messages/DrawMessage.hpp" // for DrawMessage
#include "windows/AppWindow.hpp"                // for AppWindow
#include "DOMResponder.hpp"
#include <WindowsStack.hpp>
#include <WindowsPopupFilter.hpp>
#include <service-gui/ServiceGUIName.hpp>

#include <service-db/Settings.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <service-audio/AudioServiceAPI.hpp> // for GetOutputVolume

#include <popups/data/PhoneModeParams.hpp>
#include <popups/data/BluetoothModeParams.hpp>
#include <popups/data/TetheringParams.hpp>

#if DEBUG_INPUT_EVENTS == 1
#define debug_input_events(...) LOG_DEBUG(__VA_ARGS__)
#else
#define debug_input_events(...)
#endif

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

    const char *ApplicationCommon::stateStr(ApplicationCommon::State st)
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

    ApplicationCommon::ApplicationCommon(std::string name,
                                         std::string parent,
                                         StatusIndicators statusIndicators,
                                         StartInBackground startInBackground,
                                         std::uint32_t stackDepth,
                                         sys::ServicePriority priority)
        : Service(std::move(name), std::move(parent), stackDepth, priority),
          popupFilter(std::make_unique<gui::popup::Filter>()), windowsStackImpl(std::make_unique<WindowsStack>()),
          default_window(gui::name::window::main_window),
          keyTranslator{std::make_unique<gui::KeyInputSimpleTranslation>()}, startInBackground{startInBackground},
          callbackStorage{std::make_unique<CallbackStorage>()}, statusBarManager{std::make_unique<StatusBarManager>()},
          settings(std::make_unique<settings::Settings>()), statusIndicators{statusIndicators}, phoneLockSubject(this),
          lockPolicyHandler(this), simLockSubject(this)
    {
        popupFilter->attachWindowsStack(windowsStackImpl.get());
        statusBarManager->enableIndicators({gui::status_bar::Indicator::Time,
                                            gui::status_bar::Indicator::Signal,
                                            gui::status_bar::Indicator::NetworkAccessTechnology,
                                            gui::status_bar::Indicator::PhoneMode});

        bus.channels.push_back(sys::BusChannel::ServiceCellularNotifications);

        bus.channels.push_back(sys::BusChannel::USBNotifications);

        longPressTimer = sys::TimerFactory::createPeriodicTimer(this,
                                                                "LongPress",
                                                                std::chrono::milliseconds{keyTimerMs},
                                                                [this](sys::Timer &) { longPressTimerCallback(); });

        connect(typeid(AppRefreshMessage),
                [this](sys::Message *msg) -> sys::MessagePointer { return handleAppRefresh(msg); });
        connect(sevm::BatteryStatusChangeMessage(), [&](sys::Message *) { return handleBatteryStatusChange(); });
        connect(typeid(app::manager::DOMRequest),
                [&](sys::Message *msg) -> sys::MessagePointer { return handleGetDOM(msg); });
        connect(typeid(AppUpdateWindowMessage),
                [&](sys::Message *msg) -> sys::MessagePointer { return handleUpdateWindow(msg); });
        connect(typeid(cellular::msg::notification::SimStateUpdate),
                [&](sys::Message *msg) -> sys::MessagePointer { return handleSimStateUpdateMessage(msg); });
        connect(typeid(sdesktop::usb::USBConnected),
                [&](sys::Message *msg) -> sys::MessagePointer { return handleUSBStatusChange(); });
        connect(typeid(sdesktop::usb::USBDisconnected),
                [&](sys::Message *msg) -> sys::MessagePointer { return handleUSBStatusChange(); });

        addActionReceiver(app::manager::actions::PhoneModeChanged, [this](auto &&params) {
            if (params != nullptr) {
                auto modeParams                  = static_cast<gui::PhoneModeParams *>(params.get());
                this->statusIndicators.phoneMode = modeParams->getPhoneMode();
            }
            return actionHandled();
        });
        addActionReceiver(app::manager::actions::TetheringStateChanged, [this](auto &&params) {
            if (params != nullptr) {
                auto modeParams                       = static_cast<gui::TetheringParams *>(params.get());
                this->statusIndicators.tetheringState = modeParams->getTetheringState();
            }
            return actionHandled();
        });
        addActionReceiver(app::manager::actions::BluetoothModeChanged, [this](auto &&params) {
            if (params != nullptr) {
                auto modeParams                      = static_cast<gui::BluetoothModeParams *>(params.get());
                this->statusIndicators.bluetoothMode = modeParams->getBluetoothMode();
                refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
            }
            return actionHandled();
        });
        addActionReceiver(app::manager::actions::AlarmClockStatusChanged, [this](auto &&params) {
            if (params != nullptr) {
                auto status = static_cast<AlarmClockStatusParams *>(params.get())->getAlarmClockStatus();
                this->statusIndicators.alarmClockStatus = status;
                refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
            }
            return actionHandled();
        });
        addActionReceiver(app::manager::actions::ShowPopup, [this](auto &&params) {
            actionPopupPush(std::forward<decltype(params)>(params));
            return actionHandled();
        });
        addActionReceiver(app::manager::actions::AbortPopup, [this](auto &&params) {
            auto popupParams   = static_cast<gui::PopupRequestParams *>(params.get());
            const auto popupId = popupParams->getPopupId();
            abortPopup(popupId);
            return actionHandled();
        });
        addActionReceiver(app::manager::actions::NotificationsChanged, [this](auto &&params) {
            handleNotificationsChanged(std::forward<decltype(params)>(params));
            return actionHandled();
        });

        registerPopupBlueprints();
    }

    ApplicationCommon::~ApplicationCommon() noexcept
    {
        windowsStack().clear();
    }

    ApplicationCommon::State ApplicationCommon::getState()
    {
        return state;
    }

    void ApplicationCommon::setState(State st)
    {
#if DEBUG_APPLICATION_MANAGEMENT == 1
        LOG_DEBUG("[%s] (%s) -> (%s)", GetName().c_str(), stateStr(state), stateStr(st));
#endif
        // To prevent handling of key presses event from other state of application as a longPress
        if (state != st) {
            keyTranslator->resetPreviousKeyPress();
            longPressTimer.stop();
        }
        state = st;
    }

    void ApplicationCommon::longPressTimerCallback()
    {
        const auto actualTimeStamp = xTaskGetTickCount();
        if (keyTranslator->isKeyPressTimedOut(actualTimeStamp)) {
            const auto inputEvent = keyTranslator->translate(actualTimeStamp);
            messageInputEventApplication(this, this->GetName(), inputEvent);
            keyTranslator->resetPreviousKeyPress();
            longPressTimer.stop();

            debug_input_events("AppInput -> K:|%s|, S:|%s|, App:|%s|, W:|%s|",
                               magic_enum::enum_name(inputEvent.getKeyCode()).data(),
                               magic_enum::enum_name(inputEvent.getState()).data(),
                               GetName().c_str(),
                               getCurrentWindow()->getName().c_str());
        }
    }

    void ApplicationCommon::clearLongPressTimeout()
    {
        keyTranslator->setPreviousKeyTimedOut(false);
    }

    void ApplicationCommon::render(gui::RefreshModes mode)
    {
        if (windowsStack().isEmpty()) {
            LOG_ERROR("Current window is not defined");
            return;
        }

        // send drawing commands only when if application is in active and visible.
        if (state == State::ACTIVE_FORGROUND) {
            auto window = getCurrentWindow();
            updateStatuses(window);

            auto message = std::make_shared<service::gui::DrawMessage>(window->buildDrawList(), mode);

            if (suspendInProgress) {
                message->setCommandType(service::gui::DrawMessage::Type::SUSPEND);
            }

            bus.sendUnicast(std::move(message), service::name::gui);
        }

        if (suspendInProgress) {
            suspendInProgress = false;
        }
    }

    void ApplicationCommon::updateStatuses(gui::AppWindow *window) const
    {}

    void ApplicationCommon::setDefaultWindow(const std::string &w)
    {
        default_window = w;
    }

    WindowsStack &ApplicationCommon::windowsStack() const
    {
        return *windowsStackImpl;
    }

    void ApplicationCommon::updateCurrentWindow(std::unique_ptr<gui::SwitchData> data,
                                                gui::ShowMode command,
                                                gui::RefreshModes refreshMode)
    {
        const auto currentWindow = getCurrentWindow();
        auto msg                 = std::make_shared<AppUpdateWindowMessage>(
            (currentWindow != nullptr) ? currentWindow->getName() : "", std::move(data), command, refreshMode);
        bus.sendUnicast(std::move(msg), this->GetName());
    }

    void ApplicationCommon::switchWindow(const std::string &windowName,
                                         gui::ShowMode cmd,
                                         std::unique_ptr<gui::SwitchData> data,
                                         SwitchReason reason)
    {

        std::string window;
#if DEBUG_APPLICATION_MANAGEMENT == 1
        LOG_DEBUG("Switching [%s] to window: %s data description: %s",
                  GetName().c_str(),
                  windowName.length() ? windowName.c_str() : default_window.c_str(),
                  data ? data->getDescription().c_str() : "");
#endif

        // case to handle returning to previous application
        if (windowName.empty()) {
            window   = getCurrentWindow()->getName();
            auto msg = std::make_shared<AppSwitchWindowMessage>(
                window, getCurrentWindow()->getName(), std::move(data), cmd, reason);
            bus.sendUnicast(std::move(msg), this->GetName());
        }
        else {
            auto msg = std::make_shared<AppSwitchWindowMessage>(
                windowName,
                (getCurrentWindow() != nullptr) ? getCurrentWindow()->getName() : "",
                std::move(data),
                cmd,
                reason);
            bus.sendUnicast(std::move(msg), this->GetName());
        }
    }

    void ApplicationCommon::switchWindowPopup(const std::string &windowName,
                                              const gui::popup::Disposition &d,
                                              std::unique_ptr<gui::SwitchData> data,
                                              SwitchReason reason)
    {
        auto msg = std::make_shared<AppSwitchWindowPopupMessage>(windowName, std::move(data), reason, d);
        bus.sendUnicast(std::move(msg), this->GetName());
    }

    void ApplicationCommon::returnToPreviousWindow()
    {
        auto window = windowsStack().get(previousWindow);
        if (!window.has_value()) {
            LOG_DEBUG("No window to back from - get to previous app");
            app::manager::Controller::switchBack(this);
            return;
        }
        LOG_INFO("Back to previous window: %s", window->c_str());
        windowsStack().pop(*window);

        // MOS-350: SimInfo pop-us should be ignored in windows history - pop one window deeper
        auto const simInfoName = gui::popup::resolveWindowName(gui::popup::ID::SimInfo);
        if (window == simInfoName) {
            window = windowsStack().get(previousWindow);
            // SimInfo won't arrive with no preceding window, so no need to check the optional
            LOG_INFO("Previous window is %s - omit it and go one more back: %s", simInfoName.c_str(), window->c_str());
        }

        switchWindow(*window, gui::ShowMode::GUI_SHOW_RETURN);
    }

    void ApplicationCommon::popCurrentWindow()
    {
        windowsStack().pop();
    }

    void ApplicationCommon::popWindow(const std::string &window)
    {
        windowsStack().drop(window);
    }

    void ApplicationCommon::refreshWindow(gui::RefreshModes mode)
    {
        const auto &window = windowsStack().get(topWindow);
        if (window.has_value()) {
            auto msg = std::make_shared<AppRefreshMessage>(mode, *window);
            bus.sendUnicast(std::move(msg), this->GetName());
        }
    }

    sys::MessagePointer ApplicationCommon::DataReceivedHandler(sys::DataMessage *msgl)
    {
        const auto msg = dynamic_cast<cellular::NotificationMessage *>(msgl);
        if (msg != nullptr) {
            if (msg->content == cellular::NotificationMessage::Content::SignalStrengthUpdate) {
                return handleSignalStrengthUpdate(msgl);
            }
            if (msg->content == cellular::NotificationMessage::Content::NetworkStatusUpdate) {
                return handleNetworkAccessTechnologyUpdate(msgl);
            }
        }

        switch (msgl->messageType) {
        case MessageType::AppInputEvent:
            return handleInputEvent(msgl);
        case MessageType::KBDKeyEvent:
            return handleKBDKeyEvent(msgl);
        case MessageType::EVMMinuteUpdated:
            return handleMinuteUpdated(msgl);
        case MessageType::AppAction:
            return handleAction(msgl);
        case MessageType::AppSwitch:
            return handleApplicationSwitch(msgl);
        case MessageType::AppSwitchBack:
            returnToPreviousWindow();
            return sys::msgHandled();
        case MessageType::AppSwitchWindow:
            return handleSwitchWindow(msgl);
        case MessageType::AppClose:
            return handleAppClose(msgl);
        case MessageType::AppRebuild:
            return handleAppRebuild(msg);
        case MessageType::AppFocusLost:
            return handleAppFocusLost(msgl);
        default:
            return sys::msgNotHandled();
        }
    }

    sys::MessagePointer ApplicationCommon::handleAsyncResponse(sys::ResponseMessage *resp)
    {
        if (resp == nullptr) {
            return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
        }

        if (const auto command = callbackStorage->getCallback(resp); command->execute()) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
            checkBlockingRequests();
        }
        return std::make_shared<sys::ResponseMessage>();
    }

    sys::MessagePointer ApplicationCommon::handleSignalStrengthUpdate([[maybe_unused]] sys::Message *msgl)
    {
        if ((state == State::ACTIVE_FORGROUND) && getCurrentWindow()->updateSignalStrength()) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        return sys::msgHandled();
    }

    sys::MessagePointer ApplicationCommon::handleNetworkAccessTechnologyUpdate([[maybe_unused]] sys::Message *msgl)
    {
        if ((state == State::ACTIVE_FORGROUND) && getCurrentWindow()->updateNetworkAccessTechnology()) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        return sys::msgHandled();
    }

    sys::MessagePointer ApplicationCommon::handleInputEvent(sys::Message *msgl)
    {
        const auto msg = static_cast<AppInputEventMessage *>(msgl);
        if (msg->getEvent().isKeyPress()) {
            longPressTimer.start();
        }
        else if (msg->getEvent().isShortRelease()) {
            longPressTimer.stop();
        }
        if (!windowsStack().isEmpty() && getCurrentWindow()->onInput(msg->getEvent())) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        return sys::msgHandled();
    }

    sys::MessagePointer ApplicationCommon::handleKBDKeyEvent(sys::Message *msgl)
    {
        if (this->getState() != app::ApplicationCommon::State::ACTIVE_FORGROUND) {
            LOG_FATAL("Terrible terrible damage! Application with no focus grabbed key!");
        }
        const auto msg        = static_cast<sevm::KbdMessage *>(msgl);
        const auto inputEvent = keyTranslator->translate(msg->key);
        if (!inputEvent.is(gui::KeyCode::KEY_UNDEFINED)) {
            messageInputEventApplication(this, this->GetName(), inputEvent);
        }

        debug_input_events("AppInput -> K:|%s|, S:|%s|, App:|%s|, W:|%s|",
                           magic_enum::enum_name(iev.getKeyCode()).data(),
                           magic_enum::enum_name(iev.getState()).data(),
                           GetName().c_str(),
                           getCurrentWindow()->getName().c_str());

        return sys::msgHandled();
    }

    sys::MessagePointer ApplicationCommon::handleBatteryStatusChange()
    {
        if ((state == State::ACTIVE_FORGROUND) && getCurrentWindow()->updateBatteryStatus()) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        return sys::msgHandled();
    }

    sys::MessagePointer ApplicationCommon::handleUSBStatusChange()
    {
        if ((state == State::ACTIVE_FORGROUND) && getCurrentWindow()->updateBatteryStatus()) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        return sys::msgHandled();
    }

    sys::MessagePointer ApplicationCommon::handleMinuteUpdated([[maybe_unused]] sys::Message *msgl)
    {
        if (state == State::ACTIVE_FORGROUND) {
            if (const auto requestedRefreshMode = getCurrentWindow()->updateTime();
                requestedRefreshMode != gui::RefreshModes::GUI_REFRESH_NONE) {
                refreshWindow(requestedRefreshMode);
            }
        }
        return sys::msgHandled();
    }

    sys::MessagePointer ApplicationCommon::handleAction(sys::Message *msgl)
    {
        const auto msg    = static_cast<AppActionRequest *>(msgl);
        const auto action = msg->getAction();
        try {
            const auto &actionHandler = receivers.at(action);
            auto &data                = msg->getData();

            auto result = actionHandler(std::move(data));

            if (getState() == State::ACTIVE_FORGROUND && windowsStack().isEmpty()) {
                LOG_ERROR("Application switch with no window provided. Fallback to default mainWindow.");
                pushWindow(default_window);
            }

            return result;
        }
        catch (const std::out_of_range &) {
            LOG_ERROR("Application %s is not able to handle action #%d", GetName().c_str(), action);
        }
        return actionNotHandled();
    }

    sys::MessagePointer ApplicationCommon::handleApplicationSwitch(sys::Message *msgl)
    {
        const auto msg = static_cast<AppSwitchMessage *>(msgl);

        switch (msg->getApplicationStartupReason()) {
        case StartupReason::Launch:
            return handleApplicationSwitchLaunch(msgl);
        case StartupReason::OnAction:
            return handleApplicationSwitchOnAction(msgl);
        }

        return sys::msgNotHandled();
    }

    sys::MessagePointer ApplicationCommon::handleApplicationSwitchLaunch(sys::Message *msgl)
    {
        const auto msg = static_cast<AppSwitchMessage *>(msgl);
        bool handled   = false;
        LOG_DEBUG("Application switch: %s", msg->getTargetApplicationName().c_str());
        // Application is starting or it is in the background. Upon switch command if name is correct it goes
        // foreground
        if ((state == State::ACTIVATING) || (state == State::INITIALIZING) || (state == State::ACTIVE_BACKGROUND)) {

            if (msg->getTargetApplicationName() == this->GetName()) {
                setState(State::ACTIVE_FORGROUND);
                if (app::manager::Controller::confirmSwitch(this)) {
                    LOG_DEBUG("Target window: %s : target description: %s",
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

    sys::MessagePointer ApplicationCommon::handleApplicationSwitchOnAction([[maybe_unused]] sys::Message *msgl)
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

    sys::MessagePointer ApplicationCommon::handleSwitchWindow(sys::Message *msgl)
    {
        const auto msg        = static_cast<AppSwitchWindowMessage *>(msgl);
        const auto windowName = msg->getWindowName();
        if (!windowsFactory.isRegistered(windowName)) {
            LOG_ERROR("No such window: %s", windowName.c_str());
            return sys::msgHandled();
        }

        const auto switchData = std::move(msg->getData());
        if (switchData && switchData->ignoreCurrentWindowOnStack) {
            windowsStack().pop();
        }

        const auto anotherWindowOnTop = !isCurrentWindow(windowName) && !windowsStack().isEmpty();
        if (anotherWindowOnTop) {
            auto closeReason = gui::Window::CloseReason::WindowSwitch;
            switch (msg->getReason()) {
            case SwitchReason::PhoneLock:
                closeReason = gui::Window::CloseReason::PhoneLock;
                break;
            case SwitchReason::Popup:
                closeReason = gui::Window::CloseReason::Popup;
                break;
            default:
                break;
            }
            getCurrentWindow()->onClose(closeReason);
        }

        LOG_DEBUG("Request to switch window from %s to %s", getCurrentWindow()->getName().c_str(), windowName.c_str());
        const auto &[name, data] = msg->getSwitchData();
        pushWindow(name, data);
        getCurrentWindow()->handleSwitchData(switchData.get());

        if (handleUpdateTextRefresh(switchData.get())) {
            return sys::msgHandled();
        }
        getCurrentWindow()->onBeforeShow(msg->getCommand(), switchData.get());

        /// This is kind of gentle hack on our stateless Application Window management switching
        /// if we are not requesting for popup and we cant handle next popup - just refresh the window
        /// else -> popup will refresh it for us
        if (!tryShowPopup()) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
        }
        return sys::msgHandled();
    }

    bool ApplicationCommon::handleUpdateTextRefresh(gui::SwitchData *data)
    {
        const auto ret = dynamic_cast<gui::SwitchSpecialChar *>(data);
        if ((ret != nullptr) && (ret->type == gui::SwitchSpecialChar::Type::Response)) {
            const auto text = dynamic_cast<gui::Text *>(getCurrentWindow()->getFocusItem());
            if (text != nullptr) {
                text->addText(ret->getDescription());
                refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
                return true;
            }
        }
        return false;
    }

    sys::MessagePointer ApplicationCommon::handleUpdateWindow(sys::Message *msgl)
    {
        const auto msg           = static_cast<AppUpdateWindowMessage *>(msgl);
        const auto haveBuilder   = windowsFactory.isRegistered(msg->getWindowName());
        const auto isWindowOnTop = isCurrentWindow(msg->getWindowName());
        if (haveBuilder && isWindowOnTop) {
            const auto &switchData = msg->getData();
            getCurrentWindow()->handleSwitchData(switchData.get());
            getCurrentWindow()->onBeforeShow(msg->getCommand(), switchData.get());
            refreshWindow(msg->getRefreshMode());
        }
        else {
            LOG_ERROR("Can't update window: '%s' in app: '%s', params: haveBuilder: %s is_on_top: %s",
                      msg->getWindowName().c_str(),
                      GetName().c_str(),
                      haveBuilder ? "yes" : "no",
                      isWindowOnTop ? "yes" : "no");
        }
        return sys::msgHandled();
    }

    sys::MessagePointer ApplicationCommon::handleAppClose([[maybe_unused]] sys::Message *msgl)
    {
        setState(State::DEACTIVATING);

        for (const auto &[windowName, window] : windowsStack()) {
            LOG_INFO("Closing a window: %s", windowName.c_str());
            window->onClose(gui::Window::CloseReason::ApplicationClose);
        }

        if (callbackStorage->checkBlockingCloseRequests()) {
            setState(State::FINALIZING_CLOSE);
            app::manager::Controller::finalizingClose(this);
            return sys::msgHandled();
        }

        app::manager::Controller::confirmClose(this);
        return sys::msgHandled();
    }

    void ApplicationCommon::checkBlockingRequests()
    {
        if (getState() == State::FINALIZING_CLOSE && !callbackStorage->checkBlockingCloseRequests()) {
            LOG_INFO("Blocking requests done for '%s'. Closing application.", GetName().c_str());
            setState(State::DEACTIVATING);
            app::manager::Controller::confirmClose(this);
        }
    }

    sys::MessagePointer ApplicationCommon::handleAppRebuild([[maybe_unused]] sys::Message *msgl)
    {
        LOG_INFO("Application %s rebuilding gui", GetName().c_str());
        windowsStack().rebuildWindows(windowsFactory, this);
        LOG_INFO("Refresh app with focus!");
        if (state == State::ACTIVE_FORGROUND) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
        }
        LOG_INFO("App rebuild done");
        return sys::msgHandled();
    }

    sys::MessagePointer ApplicationCommon::handleAppRefresh(sys::Message *msgl)
    {
        const auto msg = static_cast<AppRefreshMessage *>(msgl);
        assert(msg);
        if (windowsStack().isEmpty() || (getCurrentWindow()->getName() != msg->getWindowName())) {
            LOG_DEBUG("Ignore request for window %s we are on window %s",
                      msg->getWindowName().c_str(),
                      windowsStack().isEmpty() ? "none" : getCurrentWindow()->getName().c_str());
            return sys::msgNotHandled();
        }
        render(msg->getMode());
        return sys::msgHandled();
    }

    sys::MessagePointer ApplicationCommon::handleGetDOM(sys::Message *msgl)
    {
        if (windowsStack().isEmpty()) {
            LOG_ERROR("Current window is not defined - can't dump DOM");
            return sys::msgNotHandled();
        }
        const auto window = getCurrentWindow();
        if (window == nullptr) {
            LOG_ERROR("Failed to get current window");
            return sys::msgNotHandled();
        }

        const auto request = static_cast<app::manager::DOMRequest *>(msgl);
        LOG_DEBUG("Get DOM for: %s", request->getSenderName().c_str());
        bus.sendUnicast(DOMResponder(GetName(), *window, std::move(request->event)).build(), request->getSenderName());

        return sys::msgHandled();
    }

    sys::MessagePointer ApplicationCommon::handleAppFocusLost([[maybe_unused]] sys::Message *msgl)
    {
        if (state == State::ACTIVE_FORGROUND) {
            setState(State::ACTIVE_BACKGROUND);
            app::manager::Controller::confirmSwitch(this);
        }
        return sys::msgHandled();
    }

    sys::MessagePointer ApplicationCommon::handleSimStateUpdateMessage([[maybe_unused]] sys::Message *msgl)
    {
        if (getCurrentWindow()->updateSim()) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        return sys::msgHandled();
    }

    sys::ReturnCodes ApplicationCommon::InitHandler()
    {
        setState(State::INITIALIZING);

        settings->init(service::ServiceProxy(shared_from_this()));
        app::manager::Controller::applicationInitialised(this, StartupStatus::Success, startInBackground);

        if (startInBackground) {
            setState(State::ACTIVE_BACKGROUND);
        }

        return sys::ReturnCodes::Success;
    }

    sys::ReturnCodes ApplicationCommon::DeinitHandler()
    {
        settings->deinit();
        LOG_INFO("Closing an application: %s", GetName().c_str());
        LOG_DEBUG("Deleting windows");
        windowsStack().clear();
        return sys::ReturnCodes::Success;
    }

    bool ApplicationCommon::adjustCurrentVolume(const int step)
    {
        return AudioServiceAPI::KeyPressed(this, step);
    }

    void ApplicationCommon::toggleTorchOnOff()
    {
        auto msg = std::make_shared<sevm::ToggleTorchOnOffMessage>();
        bus.sendUnicast(std::move(msg), service::name::evt_manager);
    }

    void ApplicationCommon::toggleTorchColor()
    {
        auto msg = std::make_shared<sevm::ToggleTorchColorMessage>();
        bus.sendUnicast(std::move(msg), service::name::evt_manager);
    }

    void ApplicationCommon::requestAction(sys::Service *sender,
                                          const ApplicationName &applicationName,
                                          manager::actions::ActionId actionId,
                                          manager::actions::ActionParamsPtr &&data)
    {
        auto msg = std::make_shared<AppActionRequest>(actionId, std::move(data));
        sender->bus.sendUnicast(std::move(msg), applicationName);
    }

    void ApplicationCommon::messageSwitchApplication(sys::Service *sender,
                                                     const std::string &application,
                                                     const std::string &window,
                                                     std::unique_ptr<gui::SwitchData> data,
                                                     StartupReason startupReason)
    {
        auto msg = std::make_shared<AppSwitchMessage>(application, window, std::move(data), startupReason);
        sender->bus.sendUnicast(std::move(msg), application);
    }

    void ApplicationCommon::messageCloseApplication(sys::Service *sender, const std::string &application)
    {
        auto msg = std::make_shared<AppMessage>(MessageType::AppClose);
        sender->bus.sendUnicast(std::move(msg), application);
    }

    void ApplicationCommon::messageRebuildApplication(sys::Service *sender, const std::string &application)
    {
        auto msg = std::make_shared<AppRebuildMessage>();
        sender->bus.sendUnicast(std::move(msg), application);
    }

    void ApplicationCommon::messageApplicationLostFocus(sys::Service *sender, const std::string &application)
    {
        auto msg = std::make_shared<AppLostFocusMessage>();
        sender->bus.sendUnicast(std::move(msg), application);
    }

    void ApplicationCommon::messageSwitchBack(sys::Service *sender, const std::string &application)
    {
        auto msg = std::make_shared<AppSwitchBackMessage>();
        sender->bus.sendUnicast(std::move(msg), application);
    }

    void ApplicationCommon::messageInputEventApplication(sys::Service *sender,
                                                         const std::string &application,
                                                         const gui::InputEvent &event)
    {
        auto msg = std::make_shared<AppInputEventMessage>(event);
        sender->bus.sendUnicast(std::move(msg), application);
    }

    void ApplicationCommon::actionPopupPush(std::unique_ptr<gui::SwitchData> params)
    {
        auto rdata = dynamic_cast<gui::PopupRequestParams *>(params.get());
        if (rdata == nullptr) {
            assert(0 && "invalid popup data received");
            return;
        }

        // two lines below is to **not** loose data on copy on dynamic_cast, but to move the data
        (void)params.release();
        auto data                    = std::unique_ptr<gui::PopupRequestParams>(rdata);
        const auto id                = data->getPopupId();
        auto blueprint               = popupBlueprint.getBlueprint(id);

        const auto topOfWindowsStackId = windowsStack().getWindowData(app::topWindow)->disposition.id;
        if (data->ignoreIfTheSamePopupIsOnTopOfTheStack && id == topOfWindowsStackId) {
            LOG_WARN("Ignoring popup because the same is already on the top of the stack");
            return;
        }

        if (!blueprint) {
            LOG_ERROR("No blueprint to handle %s popup - fallback", std::string(magic_enum::enum_name(id)).c_str());
            blueprint = popupBlueprintFallback(id);
        }
        if (data->getDisposition().windowtype != gui::popup::Disposition::WindowType::Popup) {
            LOG_ERROR("Setting popup window type from %s to popup - fallback",
                      magic_enum::enum_name(data->getDisposition().windowtype).data());
            data->setDisposition(gui::popup::Disposition{
                gui::popup::Disposition::Priority::Normal, gui::popup::Disposition::WindowType::Popup, id});
        }
        if (popupFilter->addPopup(gui::PopupRequestParams(id))) {
            windowsPopupQueue->pushRequest(gui::popup::Request(id, std::move(data), *blueprint));
            auto result = tryShowPopup();
            LOG_INFO("Try to show Popup %s status: %s", magic_enum::enum_name(id).data(), result ? "shown" : "ignored");
        }
        else {
            LOG_INFO("Popup %s removed", magic_enum::enum_name(id).data());
        }
    }

    gui::popup::Filter &ApplicationCommon::getPopupFilter() const
    {
        return *popupFilter;
    }

    void ApplicationCommon::clearPendingPopups()
    {
        windowsPopupQueue->clear();
    }

    bool ApplicationCommon::tryShowPopup()
    {
        using namespace gui::popup;

        auto request = windowsPopupQueue->popRequest(getPopupFilter());
        if (!request.has_value()) {
            return false;
        }

        auto const id = request->getPopupParams().getPopupId();

        if (id == ID::SimInfo) {
            // MOS-350: silently get rid of the first occurence of SimLock pop-up and all subsequent windows
            windowsStack().drop(resolveWindowName(ID::SimLock));
        }

        auto const popup = magic_enum::enum_name(id).data();
        LOG_DEBUG("Handling popup: %s", popup);
        /// request handle actually switches window to popup window
        auto retval = request->handle();
        if (!retval) {
            LOG_ERROR("Popup %s handling failure, please check registered blueprint!", popup);
        }

        return retval;
    }

    void ApplicationCommon::abortPopup(gui::popup::ID id)
    {
        const auto popupName = gui::popup::resolveWindowName(id);

        /* Hack for dropping tethering confirmation window on locked phone to prevent removing
         * phone lock popup, thus unlocking the phone without providing passcode. */
        if ((id == gui::popup::ID::Tethering) &&
            windowsStack().isWindowOnStack(gui::popup::window::phone_lock_window)) {
            LOG_INFO("Aborting tethering confirmation window on locked phone");
            windowsStack().drop(popupName);
            return;
        }

        /* Regular popup aborting handler */
        LOG_INFO("Aborting popup %s from window %s", popupName.c_str(), getCurrentWindow()->getName().c_str());
        if (!windowsStack().pop(popupName)) {
            return;
        }
        if (popupName == getCurrentWindow()->getName()) {
            returnToPreviousWindow();
        }
    }

    void ApplicationCommon::requestShutdownWindow(const std::string &windowName)
    {
#if DEBUG_APPLICATION_MANAGEMENT == 1
        LOG_DEBUG("Switching [%s] to shutdown window: %s",
                  GetName().c_str(),
                  windowName.length() ? windowName.c_str() : default_window.c_str());
#endif
        if (!windowsFactory.isRegistered(windowName)) {
            LOG_ERROR("Cannot find window %s windowsFactory in application: %s", windowName.c_str(), GetName().c_str());
            return;
        }

        pushWindow(windowName);
        const auto window = getWindow(windowName);
        if (window == nullptr) {
            LOG_ERROR("Cannot find window %s in application %s", windowName.c_str(), GetName().c_str());
            return;
        }

        const auto &response = bus.sendUnicastSync(
            std::make_shared<service::gui::DrawMessage>(window->buildDrawList(), gui::RefreshModes::GUI_REFRESH_DEEP),
            service::name::gui,
            100);
        if (response.first != sys::ReturnCodes::Success) {
            LOG_FATAL("Failed to send the shutdown window frame");
        }
    }

    bool ApplicationCommon::userInterfaceDBNotification([[maybe_unused]] sys::Message *msg,
                                                        [[maybe_unused]] const UiNotificationFilter &filter)
    {
        bool handled = false;
        for (const auto &[name, window] : windowsStack()) {
            if (filter == nullptr || (filter != nullptr && filter(msg, window->getName()))) {
                handled |= window->onDatabaseMessage(msg);
            }
        }
        return handled;
    }

    void ApplicationCommon::pushWindow(const std::string &newWindow, const gui::popup::Disposition &d)
    {
        // handle if window was already on
        LOG_INFO("App: %s window %s request", GetName().c_str(), newWindow.c_str());
        if (newWindow.empty() && windowsStack().popLastWindow()) {
            LOG_WARN("Empty window request!");
            return;
        }
        if (windowsStack().pop(newWindow)) {
            LOG_INFO("Requested window %s is previous one - get back to it", newWindow.c_str());
            return;
        }
        LOG_INFO("Create window for stack: %s", newWindow.c_str());
        windowsStack().push(newWindow, windowsFactory.build(this, newWindow), d);
    }

    std::optional<std::string> ApplicationCommon::getPreviousWindow(std::uint32_t count) const
    {
        return windowsStack().get(count);
    }

    gui::AppWindow *ApplicationCommon::getCurrentWindow()
    {
        if (windowsStack().isEmpty()) {
            windowsStack().push(default_window, windowsFactory.build(this, default_window));
        }
        const auto &windowName = windowsStack().get(topWindow);
        const auto window      = windowsStack().get(*windowName);
        if (windowName != window->getName()) {
            LOG_FATAL("Factory vs Builtin name mismatch ! %s vs %s", windowName->c_str(), window->getName().c_str());
            window->setName(*windowName);
        }
        return window;
    }

    std::size_t ApplicationCommon::getSizeOfWindowsStack()
    {
        return windowsStack().getSize();
    }

    bool ApplicationCommon::isCurrentWindow(const std::string &windowName) const noexcept
    {
        if (const auto &window = windowsStack().get(topWindow); window.has_value()) {
            return window == windowName;
        }
        LOG_ERROR("No window: %s", windowName.c_str());
        return false;
    }

    bool ApplicationCommon::isPreviousWindow(const std::string &windowName) const noexcept
    {
        if (const auto &previousWindowName = getPreviousWindow(); previousWindowName.has_value()) {
            return previousWindowName == windowName;
        }

        LOG_WARN("No previous window");
        return false;
    }

    gui::AppWindow *ApplicationCommon::getWindow(const std::string &name)
    {
        return windowsStack().get(name);
    }

    void ApplicationCommon::connect(GuiTimer *timer, gui::Item *item)
    {
        item->attachTimer(timer);
    }

    const gui::status_bar::Configuration &ApplicationCommon::getStatusBarConfiguration() const noexcept
    {
        return statusBarManager->getConfiguration();
    }

    void ApplicationCommon::addActionReceiver(manager::actions::ActionId actionId, OnActionReceived &&callback)
    {
        receivers.insert_or_assign(actionId, std::move(callback));
    }

    void ApplicationCommon::handleNotificationsChanged(std::unique_ptr<gui::SwitchData> notificationsParams)
    {
        if (const auto &window = getCurrentWindow()->getName(); window == gui::popup::window::phone_lock_window) {
            const auto refreshMode = getRefreshModeFromNotifications(notificationsParams.get());
            updateCurrentWindow(std::move(notificationsParams), gui::ShowMode::GUI_SHOW_INIT, refreshMode);
        }
    }

    gui::RefreshModes ApplicationCommon::getRefreshModeFromNotifications(gui::SwitchData *notificationsParams)
    {
        const auto data = static_cast<manager::actions::NotificationsChangedParams *>(notificationsParams);
        return data->fastRefreshOnNotificationUpdate() ? gui::RefreshModes::GUI_REFRESH_FAST
                                                       : gui::RefreshModes::GUI_REFRESH_DEEP;
    }

    void ApplicationCommon::cancelCallbacks(AsyncCallbackReceiver::Ptr receiver)
    {
        callbackStorage->removeAll(receiver);
    }

    auto ApplicationCommon::getPhoneLockSubject() noexcept -> locks::PhoneLockSubject &
    {
        return phoneLockSubject;
    }

    bool ApplicationCommon::isPhoneLockEnabled() const noexcept
    {
        return (utils::getNumericValue<bool>(
            settings->getValue(settings::SystemProperties::lockScreenPasscodeIsOn, settings::SettingsScope::Global)));
    }

    auto ApplicationCommon::getLockPolicyHandler() noexcept -> locks::LockPolicyHandlerInterface &
    {
        return lockPolicyHandler;
    }

    auto ApplicationCommon::getSimLockSubject() noexcept -> locks::SimLockSubject &
    {
        return simLockSubject;
    }
} /* namespace app */
