// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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
#include <service-evtmgr/Constants.hpp>
#include <service-evtmgr/EVMessages.hpp>
#include <service-appmgr/messages/DOMRequest.hpp>
#include <service-appmgr/data/NotificationsChangedActionsParams.hpp>
#include "service-gui/messages/DrawMessage.hpp" // for DrawMessage
#include "windows/AppWindow.hpp"                // for AppWindow
#include "DOMResponder.hpp"
#include <WindowsStack.hpp>
#include <WindowsPopupFilter.hpp>
#include <service-gui/Common.hpp>

#include <service-db/Settings.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <service-audio/AudioServiceAPI.hpp> // for GetOutputVolume

#include <popups/data/PhoneModeParams.hpp>
#include <popups/data/BluetoothModeParams.hpp>

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
                                         uint32_t stackDepth,
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
        statusBarManager->enableIndicators({gui::status_bar::Indicator::Time});

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
        state = st;
    }

    void ApplicationCommon::longPressTimerCallback()
    {
        const auto actualTimeStamp = xTaskGetTickCount();
        if (keyTranslator->isKeyPressTimedOut(actualTimeStamp)) {
            gui::InputEvent iev = keyTranslator->translate(actualTimeStamp);
            messageInputEventApplication(this, this->GetName(), iev);
            keyTranslator->resetPreviousKeyPress();
            longPressTimer.stop();

            debug_input_events("AppInput -> K:|%s|, S:|%s|, App:|%s|, W:|%s|",
                               magic_enum::enum_name(iev.getKeyCode()).data(),
                               magic_enum::enum_name(iev.getState()).data(),
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

        if (suspendInProgress)
            suspendInProgress = false;
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
            currentWindow ? currentWindow->getName() : "", std::move(data), command, refreshMode);
        bus.sendUnicast(std::move(msg), this->GetName());
    }

    void ApplicationCommon::switchWindow(const std::string &windowName,
                                         gui::ShowMode cmd,
                                         std::unique_ptr<gui::SwitchData> data,
                                         SwitchReason reason)
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
            window   = getCurrentWindow()->getName();
            auto msg = std::make_shared<AppSwitchWindowMessage>(
                window, getCurrentWindow()->getName(), std::move(data), cmd, reason);
            bus.sendUnicast(msg, this->GetName());
        }
        else {
            auto msg = std::make_shared<AppSwitchWindowMessage>(
                windowName, getCurrentWindow() ? getCurrentWindow()->getName() : "", std::move(data), cmd, reason);
            bus.sendUnicast(msg, this->GetName());
        }
    }

    void ApplicationCommon::switchWindowPopup(const std::string &windowName,
                                              const gui::popup::Disposition &d,
                                              std::unique_ptr<gui::SwitchData> data,
                                              SwitchReason reason)
    {
        bus.sendUnicast(std::make_shared<AppSwitchWindowPopupMessage>(windowName, std::move(data), reason, d),
                        this->GetName());
    }

    void ApplicationCommon::returnToPreviousWindow()
    {
        auto window = windowsStack().get(previousWindow);
        if (not window) {
            LOG_DEBUG("No window to back from - get to previous app");
            app::manager::Controller::switchBack(this);
            return;
        }
        LOG_INFO("Back to previous window: %s", window->c_str());
        windowsStack().pop(*window);
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
        auto window = windowsStack().get(topWindow);
        if (window) {
            auto msg = std::make_shared<AppRefreshMessage>(mode, *window);
            bus.sendUnicast(msg, this->GetName());
        }
    }

    sys::MessagePointer ApplicationCommon::DataReceivedHandler(sys::DataMessage *msgl)
    {
        auto msg = dynamic_cast<cellular::NotificationMessage *>(msgl);
        if (msg != nullptr) {
            if (msg->content == cellular::NotificationMessage::Content::SignalStrengthUpdate) {
                return handleSignalStrengthUpdate(msgl);
            }
            if (msg->content == cellular::NotificationMessage::Content::NetworkStatusUpdate) {
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
        else if (msgl->messageType == MessageType::AppSwitchBack) {
            returnToPreviousWindow();
            return sys::msgHandled();
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
        return sys::msgNotHandled();
    }

    sys::MessagePointer ApplicationCommon::handleAsyncResponse(sys::ResponseMessage *resp)
    {
        if (resp != nullptr) {
            if (auto command = callbackStorage->getCallback(resp); command->execute()) {
                refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                checkBlockingRequests();
            }
            return std::make_shared<sys::ResponseMessage>();
        }
        else {
            return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
        }
    }

    sys::MessagePointer ApplicationCommon::handleSignalStrengthUpdate(sys::Message *msgl)
    {
        if ((state == State::ACTIVE_FORGROUND) && getCurrentWindow()->updateSignalStrength()) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        return sys::msgHandled();
    }

    sys::MessagePointer ApplicationCommon::handleNetworkAccessTechnologyUpdate(sys::Message *msgl)
    {
        if ((state == State::ACTIVE_FORGROUND) && getCurrentWindow()->updateNetworkAccessTechnology()) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        return sys::msgHandled();
    }

    sys::MessagePointer ApplicationCommon::handleInputEvent(sys::Message *msgl)
    {
        AppInputEventMessage *msg = reinterpret_cast<AppInputEventMessage *>(msgl);
        if (msg->getEvent().isKeyPress()) {
            longPressTimer.start();
        }
        else if (msg->getEvent().isShortRelease()) {
            longPressTimer.stop();
        }
        if (not windowsStack().isEmpty() && getCurrentWindow()->onInput(msg->getEvent())) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        return sys::msgHandled();
    }

    sys::MessagePointer ApplicationCommon::handleKBDKeyEvent(sys::Message *msgl)
    {
        if (this->getState() != app::ApplicationCommon::State::ACTIVE_FORGROUND) {
            LOG_FATAL("!!! Terrible terrible damage! application with no focus grabbed key!");
        }
        sevm::KbdMessage *msg = static_cast<sevm::KbdMessage *>(msgl);
        gui::InputEvent iev   = keyTranslator->translate(msg->key);
        if (!iev.is(gui::KeyCode::KEY_UNDEFINED)) {
            messageInputEventApplication(this, this->GetName(), iev);
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

    sys::MessagePointer ApplicationCommon::handleMinuteUpdated(sys::Message *msgl)
    {
        if (state == State::ACTIVE_FORGROUND) {
            if (auto reqestedRefreshMode = getCurrentWindow()->updateTime();
                reqestedRefreshMode != gui::RefreshModes::GUI_REFRESH_NONE) {
                refreshWindow(reqestedRefreshMode);
            }
        }
        return sys::msgHandled();
    }

    sys::MessagePointer ApplicationCommon::handleAction(sys::Message *msgl)
    {
        auto *msg         = static_cast<AppActionRequest *>(msgl);
        const auto action = msg->getAction();
        try {
            const auto &actionHandler = receivers.at(action);
            auto &data                = msg->getData();

            auto result = actionHandler(std::move(data));

            if (getState() == State::ACTIVE_FORGROUND && windowsStack().isEmpty()) {
                LOG_ERROR("OnAction application switch with no window provided. Fallback to default mainWindow.");
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
        auto *msg = static_cast<AppSwitchMessage *>(msgl);

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

    sys::MessagePointer ApplicationCommon::handleApplicationSwitchOnAction(sys::Message *msgl)
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
        if (not windowsFactory.isRegistered(windowName)) {
            LOG_ERROR("No such window: %s", windowName.c_str());
            return sys::msgHandled();
        }

        auto switchData = std::move(msg->getData());
        if (switchData && switchData->ignoreCurrentWindowOnStack) {
            windowsStack().pop();
        }
        auto anotherWindowOnTop = (not isCurrentWindow(windowName)) and (not windowsStack().isEmpty());
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

        LOG_DEBUG("Current window: %s vs %s", getCurrentWindow()->getName().c_str(), windowName.c_str());
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
        if (not tryShowPopup()) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
        }
        return sys::msgHandled();
    }

    bool ApplicationCommon::handleUpdateTextRefresh(gui::SwitchData *data)
    {
        auto ret = dynamic_cast<gui::SwitchSpecialChar *>(data);
        if (ret != nullptr && ret->type == gui::SwitchSpecialChar::Type::Response) {
            auto text = dynamic_cast<gui::Text *>(getCurrentWindow()->getFocusItem());
            if (text != nullptr) {
                text->addText(ret->getDescription());
                refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
                return true;
            }
        }
        return false;
    }

    sys::MessagePointer ApplicationCommon::handleUpdateWindow(sys::Message *msgl)
    {
        auto msg         = static_cast<AppUpdateWindowMessage *>(msgl);
        auto haveBuilder = windowsFactory.isRegistered(msg->getWindowName());
        auto window      = isCurrentWindow(msg->getWindowName());
        if (haveBuilder && window) {
            const auto &switchData = msg->getData();
            getCurrentWindow()->handleSwitchData(switchData.get());
            getCurrentWindow()->onBeforeShow(msg->getCommand(), switchData.get());
            refreshWindow(msg->getRefreshMode());
        }
        else {
            LOG_ERROR("cant update window: %s in app: %s, params: haveBuilder: %s is_on_top: %s",
                      msg->getWindowName().c_str(),
                      GetName().c_str(),
                      haveBuilder ? "yes" : "no",
                      window ? "yes" : "no");
        }
        return sys::msgHandled();
    }

    sys::MessagePointer ApplicationCommon::handleAppClose(sys::Message *msgl)
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
        else {
            app::manager::Controller::confirmClose(this);
            return sys::msgHandled();
        }
    }

    void ApplicationCommon::checkBlockingRequests()
    {
        if (getState() == State::FINALIZING_CLOSE && !callbackStorage->checkBlockingCloseRequests()) {
            LOG_INFO("Blocking requests done for [%s]. Closing application.", GetName().c_str());
            setState(State::DEACTIVATING);
            app::manager::Controller::confirmClose(this);
        }
    }

    sys::MessagePointer ApplicationCommon::handleAppRebuild(sys::Message *msgl)
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
        auto *msg = static_cast<AppRefreshMessage *>(msgl);
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

    sys::MessagePointer ApplicationCommon::handleAppFocusLost(sys::Message *msgl)
    {
        if (state == State::ACTIVE_FORGROUND) {
            setState(State::ACTIVE_BACKGROUND);
            app::manager::Controller::confirmSwitch(this);
        }
        return sys::msgHandled();
    }

    sys::MessagePointer ApplicationCommon::handleSimStateUpdateMessage(sys::Message *msgl)
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
        LOG_INFO("Deleting windows");
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
        sender->bus.sendUnicast(msg, applicationName);
    }

    void ApplicationCommon::messageSwitchApplication(sys::Service *sender,
                                                     std::string application,
                                                     std::string window,
                                                     std::unique_ptr<gui::SwitchData> data,
                                                     StartupReason startupReason)
    {
        auto msg = std::make_shared<AppSwitchMessage>(application, window, std::move(data), startupReason);
        sender->bus.sendUnicast(msg, application);
    }

    void ApplicationCommon::messageCloseApplication(sys::Service *sender, std::string application)
    {
        auto msg = std::make_shared<AppMessage>(MessageType::AppClose);
        sender->bus.sendUnicast(msg, application);
    }

    void ApplicationCommon::messageRebuildApplication(sys::Service *sender, std::string application)
    {
        auto msg = std::make_shared<AppRebuildMessage>();
        sender->bus.sendUnicast(msg, application);
    }

    void ApplicationCommon::messageApplicationLostFocus(sys::Service *sender, std::string application)
    {
        sender->bus.sendUnicast(std::make_shared<AppLostFocusMessage>(), application);
    }

    void ApplicationCommon::messageSwitchBack(sys::Service *sender, const std::string &application)
    {
        sender->bus.sendUnicast(std::make_shared<AppSwitchBackMessage>(), application);
    }

    void ApplicationCommon::messageInputEventApplication(sys::Service *sender,
                                                         std::string application,
                                                         const gui::InputEvent &event)
    {
        auto msg = std::make_shared<AppInputEventMessage>(event);
        sender->bus.sendUnicast(msg, application);
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
        auto data      = std::unique_ptr<gui::PopupRequestParams>(rdata);
        auto id        = data->getPopupId();
        auto blueprint = popupBlueprint.getBlueprint(id);
        if (!blueprint) {
            LOG_ERROR("no blueprint to handle %s popup - fallback", std::string(magic_enum::enum_name(id)).c_str());
            blueprint = popupBlueprintFallback(id);
        }
        if (data->getDisposition().windowtype != gui::popup::Disposition::WindowType::Popup) {
            LOG_ERROR("setting popup window type to popup - fallback");
            data->setDisposition(gui::popup::Disposition{
                gui::popup::Disposition::Priority::Normal, gui::popup::Disposition::WindowType::Popup, id});
        }
        windowsPopupQueue->pushRequest(gui::popup::Request(id, std::move(data), *blueprint));
        auto result = tryShowPopup();
        LOG_INFO("tryShowPopup %s status: %s", magic_enum::enum_name(id).data(), result ? "shown" : "ignored");
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
        auto request = windowsPopupQueue->popRequest(getPopupFilter());
        if (request) {
            const auto popup = magic_enum::enum_name(request->getPopupParams().getPopupId()).data();
            LOG_DEBUG("handling popup: %s", popup);
            /// request handle actually switches window to popup window
            auto retval = request->handle();
            if (not retval) {
                LOG_ERROR("Popup %s handling failure, please check registered blueprint!", popup);
            }
            return retval;
        }
        return false;
    }

    void ApplicationCommon::abortPopup(gui::popup::ID id)
    {
        const auto popupName = gui::popup::resolveWindowName(id);
        LOG_INFO("abort popup: %s from window %s", popupName.c_str(), getCurrentWindow()->getName().c_str());
        if (not windowsStack().pop(popupName)) {
            return;
        }
        if (popupName == getCurrentWindow()->getName()) {
            returnToPreviousWindow();
        }
    }

    void ApplicationCommon::requestShutdownWindow(std::string windowName)
    {
#if DEBUG_APPLICATION_MANAGEMENT == 1
        LOG_INFO("switching [%s] to shutdown window: %s",
                 GetName().c_str(),
                 windowName.length() ? windowName.c_str() : default_window.c_str());
#endif
        if (not windowsFactory.isRegistered(windowName)) {
            LOG_ERROR("Cannot find window %s windowsFactory in application: %s", windowName.c_str(), GetName().c_str());
            return;
        }

        pushWindow(windowName);
        const auto window = getWindow(windowName);
        if (not window) {
            LOG_ERROR("Cannot find window %s in application %s", windowName.c_str(), GetName().c_str());
            return;
        }

        const auto response = bus.sendUnicastSync(
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
            LOG_INFO("Empty window request!");
            return;
        }
        if (windowsStack().pop(newWindow)) {
            LOG_INFO("Get back to window!");
            return;
        }
        LOG_INFO("Create window for stack: %s", newWindow.c_str());
        windowsStack().push(newWindow, windowsFactory.build(this, newWindow), d);
    }

    std::optional<std::string> ApplicationCommon::getPrevWindow(uint32_t count) const
    {
        return windowsStack().get(count);
    }

    gui::AppWindow *ApplicationCommon::getCurrentWindow()
    {
        if (windowsStack().isEmpty()) {
            windowsStack().push(default_window, windowsFactory.build(this, default_window));
        }
        auto windowname = windowsStack().get(topWindow);
        auto window     = windowsStack().get(*windowname);
        if (windowname != window->getName()) {
            LOG_FATAL("Factory vs Builtin name mismatch ! %s vs %s", windowname->c_str(), window->getName().c_str());
            window->setName(*windowname);
        }
        return window;
    }

#pragma GCC push_options
#pragma GCC optimize ("O0")
    bool ApplicationCommon::isCurrentWindow(const std::string &windowName) const noexcept
    {
        if (const auto &window = windowsStack().get(topWindow); window != std::nullopt) {
            volatile std::string dbgName = window.value();
            volatile char const *dbgPlainName = dbgName.c_str();
            return window == windowName;
        }
        LOG_ERROR("no window: %s", windowName.c_str());
        return false;
    }
#pragma GCC pop_options

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
        if (auto window = getCurrentWindow()->getName(); window == gui::popup::window::phone_lock_window) {

            auto refreshMode = getRefreshModeFromNotifications(notificationsParams.get());
            updateCurrentWindow(std::move(notificationsParams), gui::ShowMode::GUI_SHOW_INIT, refreshMode);
        }
    }

    gui::RefreshModes ApplicationCommon::getRefreshModeFromNotifications(gui::SwitchData *notificationsParams)
    {
        auto data = static_cast<manager::actions::NotificationsChangedParams *>(notificationsParams);
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
