// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationCommon.hpp"
#include "Common.hpp"              // for RefreshModes
#include "GuiTimer.hpp"            // for GuiTimer
#include "Item.hpp"                // for Item
#include "MessageType.hpp"         // for MessageType
#include "Timers/TimerFactory.hpp" // for Timer
#include "StatusBar.hpp"
#include "status-bar/Time.hpp"
#include "Translator.hpp" // for KeyInputSim...
#include <EventStore.hpp> // for Battery
#include <hal/key_input/RawKey.hpp>
#include "gui/input/InputEvent.hpp"      // for InputEvent
#include <log/debug.hpp>                 // for DEBUG_APPLI...
#include <log/log.hpp>                   // for LOG_INFO
#include "messages/AppMessage.hpp"       // for AppSwitchMe...
#include "service-appmgr/Controller.hpp" // for Controller
#include "actions/AlarmClockStatusChangeParams.hpp"
#include <service-cellular-api>
#include <service-cellular/CellularMessage.hpp>
#include <service-evtmgr/BatteryMessages.hpp>
#include <service-evtmgr/Constants.hpp>
#include <service-evtmgr/EVMessages.hpp>
#include <service-appmgr/messages/DOMRequest.hpp>
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
#include <Utils.hpp>
#include <service-db/Settings.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>
#include <service-audio/AudioServiceAPI.hpp> // for GetOutputVolume

#include <popups/AlarmPopup.hpp>
#include <popups/VolumeWindow.hpp>
#include <popups/HomeModesWindow.hpp>
#include <popups/TetheringPhoneModePopup.hpp>
#include <popups/TetheringConfirmationPopup.hpp>
#include <popups/PowerOffWindow.hpp>
#include <popups/presenter/PowerOffPresenter.hpp>
#include <popups/lock-popups/PhoneLockedWindow.hpp>
#include <popups/lock-popups/PhoneLockedInfoWindow.hpp>
#include <popups/lock-popups/PhoneLockInputWindow.hpp>
#include <popups/lock-popups/PhoneLockChangeInfoWindow.hpp>
#include <popups/lock-popups/SimLockInputWindow.hpp>
#include <popups/lock-popups/SimInfoWindow.hpp>
#include <popups/lock-popups/SimNotReadyWindow.hpp>
#include <popups/data/PopupData.hpp>
#include <popups/data/PopupRequestParams.hpp>
#include <popups/data/AlarmPopupRequestParams.hpp>
#include <popups/data/PhoneModeParams.hpp>
#include <popups/data/BluetoothModeParams.hpp>
#include <locks/data/LockData.hpp>

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
          default_window(gui::name::window::main_window), windowsStack(this),
          keyTranslator{std::make_unique<gui::KeyInputSimpleTranslation>()}, startInBackground{startInBackground},
          callbackStorage{std::make_unique<CallbackStorage>()}, statusBarManager{std::make_unique<StatusBarManager>()},
          settings(std::make_unique<settings::Settings>()), statusIndicators{statusIndicators}, phoneLockSubject(this),
          lockPolicyHandler(this), simLockSubject(this)
    {
        statusBarManager->enableIndicators({gui::status_bar::Indicator::Time});

        bus.channels.push_back(sys::BusChannel::ServiceCellularNotifications);

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

        addActionReceiver(app::manager::actions::PhoneModeChanged, [this](auto &&params) {
            if (params != nullptr) {
                auto modeParams = static_cast<gui::PhoneModeParams *>(params.get());
                this->statusIndicators.phoneMode = modeParams->getPhoneMode();
            }
            return actionHandled();
        });
        addActionReceiver(app::manager::actions::BluetoothModeChanged, [this](auto &&params) {
            if (params != nullptr) {
                auto modeParams     = static_cast<gui::BluetoothModeParams *>(params.get());
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
            handleNotificationsChanged(std::move(params));
            return actionHandled();
        });
    }

    ApplicationCommon::~ApplicationCommon() noexcept
    {
        windowsStack.windows.clear();
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
        }
    }

    void ApplicationCommon::clearLongPressTimeout()
    {
        keyTranslator->setPreviousKeyTimedOut(false);
    }

    void ApplicationCommon::render(gui::RefreshModes mode)
    {
        if (windowsStack.isEmpty()) {
            LOG_ERROR("Current window is not defined");
            return;
        }

        // send drawing commands only when if application is in active and visible.
        if (state == State::ACTIVE_FORGROUND) {
            auto window = getCurrentWindow();
            window->updateBatteryStatus();
            window->updateBluetooth(statusIndicators.bluetoothMode);
            window->updateAlarmClock(statusIndicators.alarmClockStatus);
            window->updateSim();
            window->updateSignalStrength();
            window->updateNetworkAccessTechnology();
            window->updateTime();
            window->updatePhoneMode(statusIndicators.phoneMode);

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

    void ApplicationCommon::updateWindow(const std::string &windowName, std::unique_ptr<gui::SwitchData> data)
    {
        const auto currentWindow = getCurrentWindow();
        auto msg =
            std::make_shared<AppUpdateWindowMessage>(currentWindow ? currentWindow->getName() : "", std::move(data));
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

    void ApplicationCommon::returnToPreviousWindow(const uint32_t times)
    {
        if (const auto prevWindow = getPrevWindow(times); prevWindow == gui::name::window::no_window) {
            app::manager::Controller::switchBack(this);
        }
        else {
            LOG_INFO("Back to previous window %s", prevWindow.c_str());
            switchWindow(prevWindow, gui::ShowMode::GUI_SHOW_RETURN);
        }
    }

    void ApplicationCommon::popCurrentWindow()
    {
        if (!windowsStack.stack.empty()) {
            windowsStack.stack.pop_back();
        }
    }

    void ApplicationCommon::popWindow(const std::string &window)
    {
        auto popWindow = std::find(windowsStack.stack.begin(), windowsStack.stack.end(), window);
        if (popWindow != windowsStack.stack.end()) {
            windowsStack.stack.erase(popWindow);
        }
    }

    void ApplicationCommon::refreshWindow(gui::RefreshModes mode)
    {
        if (not windowsStack.isEmpty()) {
            auto msg = std::make_shared<AppRefreshMessage>(mode, getCurrentWindow()->getName());
            bus.sendUnicast(msg, this->GetName());
        }
    }

    sys::MessagePointer ApplicationCommon::DataReceivedHandler(sys::DataMessage *msgl)
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
        if (not windowsStack.isEmpty() && getCurrentWindow()->onInput(msg->getEvent())) {
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
        return sys::msgHandled();
    }

    sys::MessagePointer ApplicationCommon::handleBatteryStatusChange()
    {
        if ((state == State::ACTIVE_FORGROUND) && getCurrentWindow()->updateBatteryStatus()) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        return sys::msgHandled();
    }

    sys::MessagePointer ApplicationCommon::handleMinuteUpdated(sys::Message *msgl)
    {
        if (state == State::ACTIVE_FORGROUND && getCurrentWindow()->updateTime()) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
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

            if (getState() == State::ACTIVE_FORGROUND && windowsStack.isEmpty()) {
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
        auto msg = static_cast<AppSwitchWindowMessage *>(msgl);
        // check if specified window is in the application

        if (windowsFactory.isRegistered(msg->getWindowName())) {
            auto switchData = std::move(msg->getData());
            if (switchData && switchData->ignoreCurrentWindowOnStack) {
                popToWindow(getPrevWindow());
            }
            if (!isCurrentWindow(msg->getWindowName())) {
                if (!windowsStack.isEmpty()) {
                    const auto closeReason = msg->getReason() == SwitchReason::PhoneLock
                                                 ? gui::Window::CloseReason::PhoneLock
                                                 : gui::Window::CloseReason::WindowSwitch;
                    getCurrentWindow()->onClose(closeReason);
                }
                setActiveWindow(msg->getWindowName());
            }
            LOG_DEBUG("Current window: %s vs %s", getCurrentWindow()->getName().c_str(), msg->getWindowName().c_str());
            getCurrentWindow()->handleSwitchData(switchData.get());

            auto ret = dynamic_cast<gui::SwitchSpecialChar *>(switchData.get());
            if (ret != nullptr && ret->type == gui::SwitchSpecialChar::Type::Response) {
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

    sys::MessagePointer ApplicationCommon::handleUpdateWindow(sys::Message *msgl)
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

    sys::MessagePointer ApplicationCommon::handleAppClose(sys::Message *msgl)
    {
        setState(State::DEACTIVATING);

        for (const auto &[windowName, window] : windowsStack) {
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

    sys::MessagePointer ApplicationCommon::handleAppRefresh(sys::Message *msgl)
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

    sys::MessagePointer ApplicationCommon::handleGetDOM(sys::Message *msgl)
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
        windowsStack.windows.clear();
        return sys::ReturnCodes::Success;
    }

    void ApplicationCommon::setActiveWindow(const std::string &windowName)
    {
        pushWindow(windowName);
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

    void ApplicationCommon::handlePhoneModeChanged(sys::phone_modes::PhoneMode mode)
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

    void ApplicationCommon::handleVolumeChanged(audio::Volume volume, audio::Context context)
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

    void ApplicationCommon::attachPopups(const std::vector<gui::popup::ID> &popupsList)
    {
        using namespace gui::popup;
        for (auto popup : popupsList) {
            switch (popup) {
            case ID::Volume:
                windowsFactory.attach(window::volume_window, [](ApplicationCommon *app, const std::string &name) {
                    return std::make_unique<gui::VolumeWindow>(app, window::volume_window);
                });
                break;
            case ID::Tethering:
            case ID::TetheringPhoneModeChangeProhibited:
                windowsFactory.attach(window::tethering_confirmation_window,
                                      [](ApplicationCommon *app, const std::string &name) {
                                          return std::make_unique<gui::TetheringConfirmationPopup>(
                                              app, window::tethering_confirmation_window);
                                      });
                windowsFactory.attach(window::tethering_phonemode_change_window,
                                      [](ApplicationCommon *app, const std::string &name) {
                                          return std::make_unique<gui::TetheringPhoneModePopup>(
                                              app, window::tethering_phonemode_change_window);
                                      });
                break;
            case ID::PhoneModes:
                windowsFactory.attach(window::phone_modes_window, [](ApplicationCommon *app, const std::string &name) {
                    return std::make_unique<gui::HomeModesWindow>(app, window::phone_modes_window);
                });
                break;
            case ID::Brightness:
                break;
            case ID::PhoneLock:
            case ID::PhoneLockInput:
            case ID::PhoneLockInfo:
            case ID::PhoneLockChangeInfo:
                windowsFactory.attach(window::phone_lock_window, [](ApplicationCommon *app, const std::string &name) {
                    return std::make_unique<gui::PhoneLockedWindow>(app, window::phone_lock_window);
                });
                windowsFactory.attach(
                    window::phone_lock_info_window, [](ApplicationCommon *app, const std::string &name) {
                        return std::make_unique<gui::PhoneLockedInfoWindow>(app, window::phone_lock_info_window);
                    });
                windowsFactory.attach(
                    window::phone_lock_input_window, [](ApplicationCommon *app, const std::string &name) {
                        return std::make_unique<gui::PhoneLockInputWindow>(app, window::phone_lock_input_window);
                    });
                windowsFactory.attach(window::phone_lock_change_info_window,
                                      [](ApplicationCommon *app, const std::string &name) {
                                          return std::make_unique<gui::PhoneLockChangeInfoWindow>(
                                              app, window::phone_lock_change_info_window);
                                      });
                windowsFactory.attach(window::power_off_window, [](ApplicationCommon *app, const std::string &name) {
                    auto presenter = std::make_unique<gui::PowerOffPresenter>(app);
                    return std::make_unique<gui::PowerOffWindow>(app, std::move(presenter));
                });
                break;
            case ID::SimLock:
            case ID::SimInfo:
            case ID::SimNotReady:
                windowsFactory.attach(window::sim_unlock_window, [](ApplicationCommon *app, const std::string &name) {
                    return std::make_unique<gui::SimLockInputWindow>(app, window::sim_unlock_window);
                });
                windowsFactory.attach(window::sim_info_window, [](ApplicationCommon *app, const std::string &name) {
                    return std::make_unique<gui::SimInfoWindow>(app, window::sim_info_window);
                });
                windowsFactory.attach(
                    window::sim_not_ready_window, [](ApplicationCommon *app, const std::string &name) {
                        return std::make_unique<gui::SimNotReadyWindow>(app, window::sim_not_ready_window);
                    });
                break;
            case ID::Alarm:
                windowsFactory.attach(window::alarm_window, [](ApplicationCommon *app, const std::string &name) {
                    auto presenter = std::make_shared<popup::AlarmPopupPresenter>(app);
                    return std::make_unique<gui::AlarmPopup>(app, window::alarm_window, presenter);
                });
            default:
                break;
            }
        }
    }

    void ApplicationCommon::showPopup(gui::popup::ID id, const gui::PopupRequestParams *params)
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
        else if (id == ID::PhoneLock) {
            switchWindow(
                gui::popup::resolveWindowName(id), gui::ShowMode::GUI_SHOW_INIT, nullptr, SwitchReason::PhoneLock);
        }
        else if (id == ID::PhoneLockInput || id == ID::PhoneLockChangeInfo) {
            auto popupParams = static_cast<const gui::PhoneUnlockInputRequestParams *>(params);

            switchWindow(
                gui::popup::resolveWindowName(id),
                std::make_unique<locks::LockData>(popupParams->getLock(), popupParams->getPhoneLockInputTypeAction()));
        }
        else if (id == ID::SimLock || id == ID::SimInfo) {
            auto popupParams = static_cast<const gui::SimUnlockInputRequestParams *>(params);

            switchWindow(gui::popup::resolveWindowName(id),
                         std::make_unique<locks::SimLockData>(popupParams->getLock(),
                                                              popupParams->getSimInputTypeAction(),
                                                              popupParams->getErrorCode()));
        }
        else if (id == ID::Alarm) {
            auto popupParams =
                const_cast<gui::AlarmPopupRequestParams *>(static_cast<const gui::AlarmPopupRequestParams *>(params));
            switchWindow(gui::popup::resolveWindowName(id),
                         std::make_unique<gui::AlarmPopupRequestParams>(popupParams));
        }
        else {
            switchWindow(gui::popup::resolveWindowName(id));
        }
    }

    void ApplicationCommon::abortPopup(gui::popup::ID id)
    {
        const auto popupName = gui::popup::resolveWindowName(id);

        if (getCurrentWindow()->getName() == popupName) {
            returnToPreviousWindow();
        }
        else {
            popWindow(popupName);
        }
    }

    bool ApplicationCommon::isPopupPermitted([[maybe_unused]] gui::popup::ID popupId) const
    {
        return true;
    }

    bool ApplicationCommon::popToWindow(const std::string &window)
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

    void ApplicationCommon::pushWindow(const std::string &newWindow)
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

    const std::string ApplicationCommon::getPrevWindow(uint32_t count) const
    {
        if (this->windowsStack.stack.size() <= 1 || count > this->windowsStack.stack.size()) {
            return gui::name::window::no_window;
        }
        return *std::prev(windowsStack.stack.end(), count + 1);
    }

    gui::AppWindow *ApplicationCommon::getCurrentWindow()
    {
        if (windowsStack.stack.size() == 0) {
            windowsStack.push(default_window, windowsFactory.build(this, default_window));
        }
        /// TODO handle nullptr? if not found on stack -> return default
        return windowsStack.get(windowsStack.stack.back());
    }

    bool ApplicationCommon::isCurrentWindow(const std::string &windowName) const noexcept
    {
        if (windowsStack.isEmpty()) {
            return false;
        }
        return windowsStack.stack.back() == windowName;
    }

    gui::AppWindow *ApplicationCommon::getWindow(const std::string &name)
    {
        return windowsStack.get(name);
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
            switchWindow(window, std::move(notificationsParams));
        }
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
