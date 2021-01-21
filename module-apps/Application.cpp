﻿// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Application.hpp"
#include "Common.hpp"                    // for RefreshModes
#include "GuiTimer.hpp"                  // for GuiTimer
#include "Item.hpp"                      // for Item
#include "MessageType.hpp"               // for MessageType
#include "Service/Timer.hpp"             // for Timer
#include "Timer.hpp"                     // for Timer
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
#include "service-gui/messages/DrawMessage.hpp" // for DrawMessage
#include "task.h"                               // for xTaskGetTic...
#include "windows/AppWindow.hpp"                // for AppWindow
#include <Text.hpp>                             // for Text
#include <algorithm>                            // for find
#include <iterator>                             // for distance, next
#include <type_traits>                          // for add_const<>...
#include <WindowsFactory.hpp>
#include <service-gui/Common.hpp>
#include <module-utils/Utils.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>

#include <service-audio/AudioServiceAPI.hpp> // for GetOutputVolume

namespace gui
{
    class DrawCommand;
}

namespace app
{

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
                             StartInBackground startInBackground,
                             uint32_t stackDepth,
                             sys::ServicePriority priority)
        : Service(std::move(name), std::move(parent), stackDepth, priority),
          default_window(gui::name::window::main_window), windowsStack(this),
          keyTranslator{std::make_unique<gui::KeyInputSimpleTranslation>()}, startInBackground{startInBackground},
          callbackStorage{std::make_unique<CallbackStorage>()}, topBarManager{std::make_unique<TopBarManager>()},
          settings(std::make_unique<settings::Settings>(this))
    {
        topBarManager->enableIndicators({gui::top_bar::Indicator::Time});
        busChannels.push_back(sys::BusChannels::ServiceCellularNotifications);

        longPressTimer = std::make_unique<sys::Timer>("LongPress", this, key_timer_ms);
        longPressTimer->connect([&](sys::Timer &) { longPressTimerCallback(); });

        connect(typeid(AppRefreshMessage),
                [this](sys::Message *msg) -> sys::MessagePointer { return handleAppRefresh(msg); });

        settings->registerValueChange(
            settings::SystemProperties::timeFormat12,
            [this](std::string value) { timeFormatChanged(value); },
            settings::SettingsScope::Global);
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
            longPressTimer->stop();
        }
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
            if (Store::Battery::get().state == Store::Battery::State::Charging) {
                window->updateBatteryCharger(true);
            }
            else {
                window->updateBatteryLevel(Store::Battery::get().level);
            }
            window->setSIM();
            window->updateSignalStrength();
            window->updateNetworkAccessTechnology();

            auto message = std::make_shared<service::gui::DrawMessage>(window->buildDrawList(), mode);
            if (shutdownInProgress) {
                message->setCommandType(service::gui::DrawMessage::Type::SHUTDOWN);
            }
            else if (suspendInProgress) {
                message->setCommandType(service::gui::DrawMessage::Type::SUSPEND);
            }
            sys::Bus::SendUnicast(message, service::name::gui, this);
        }

        if (suspendInProgress)
            suspendInProgress = false;
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
            sys::Bus::SendUnicast(msg, this->GetName(), this);
        }
        else {
            auto msg = std::make_shared<AppSwitchWindowMessage>(
                windowName, getCurrentWindow() ? getCurrentWindow()->getName() : "", std::move(data), cmd);
            sys::Bus::SendUnicast(msg, this->GetName(), this);
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
            sys::Bus::SendUnicast(msg, this->GetName(), this);
        }
    }

    sys::MessagePointer Application::DataReceivedHandler(sys::DataMessage *msgl)
    {
        auto msg = dynamic_cast<CellularNotificationMessage *>(msgl);
        if (msg != nullptr) {
            if (msg->type == CellularNotificationMessage::Type::SignalStrengthUpdate) {
                return handleSignalStrengthUpdate(msgl);
            }
            if (msg->type == CellularNotificationMessage::Type::NetworkStatusUpdate) {
                return handleNetworkAccessTechnologyUpdate(msgl);
            }
        }
        else if (msgl->messageType == MessageType::AppInputEvent) {
            return handleInputEvent(msgl);
        }
        else if (msgl->messageType == MessageType::KBDKeyEvent) {
            return handleKBDKeyEvent(msgl);
        }
        else if (msgl->messageType == MessageType::EVMBatteryLevel) {
            return handleBatteryLevel(msgl);
        }
        else if (msgl->messageType == MessageType::EVMChargerPlugged) {
            return handleChargerPlugged(msgl);
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
        else if (auto msg = dynamic_cast<AudioKeyPressedResponse *>(msgl)) {
            if (!msg->muted) {
                // Popup should show up
                LOG_ERROR("Popup with volume level %s", std::to_string(msg->volume).c_str());
            }
        }
        return msgNotHandled();
    }

    sys::MessagePointer Application::handleSignalStrengthUpdate(sys::Message *msgl)
    {
        if ((state == State::ACTIVE_FORGROUND) && getCurrentWindow()->updateSignalStrength()) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        return msgHandled();
    }

    sys::MessagePointer Application::handleNetworkAccessTechnologyUpdate(sys::Message *msgl)
    {
        if ((state == State::ACTIVE_FORGROUND) && getCurrentWindow()->updateNetworkAccessTechnology()) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        return msgHandled();
    }

    sys::MessagePointer Application::handleInputEvent(sys::Message *msgl)
    {
        AppInputEventMessage *msg = reinterpret_cast<AppInputEventMessage *>(msgl);
        if (msg->getEvent().state == gui::InputEvent::State::keyPressed) {
            longPressTimer->start();
        }
        else if (msg->getEvent().state == gui::InputEvent::State::keyReleasedShort) {
            longPressTimer->stop();
        }
        if (not windowsStack.isEmpty() && getCurrentWindow()->onInput(msg->getEvent())) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        return msgHandled();
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
        return msgHandled();
    }

    sys::MessagePointer Application::handleBatteryLevel(sys::Message *msgl)
    {
        auto msg = static_cast<sevm::BatteryLevelMessage *>(msgl);
        LOG_INFO("Battery level: %d", msg->levelPercents);

        if (getCurrentWindow()->updateBatteryLevel(msg->levelPercents)) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        return msgHandled();
    }

    sys::MessagePointer Application::handleChargerPlugged(sys::Message *msgl)
    {
        auto *msg = static_cast<sevm::BatteryPlugMessage *>(msgl);
        if (msg->plugged == true) {
            LOG_INFO("Charger connected");
        }
        else {
            LOG_INFO("Charger disconnected");
        }
        getCurrentWindow()->updateBatteryCharger(msg->plugged);
        refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        return msgHandled();
    }

    sys::MessagePointer Application::handleMinuteUpdated(sys::Message *msgl)
    {
        auto *msg = static_cast<sevm::RtcMinuteAlarmMessage *>(msgl);
        getCurrentWindow()->updateTime(msg->timestamp, !timeFormat12);
        if (state == State::ACTIVE_FORGROUND) {
            refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }
        return msgHandled();
    }

    sys::MessagePointer Application::handleAction(sys::Message *msgl)
    {
        auto *msg         = static_cast<AppActionRequest *>(msgl);
        const auto action = msg->getAction();
        try {
            const auto &actionHandler = receivers.at(action);
            auto &data                = msg->getData();
            actionHandler(std::move(data));
        }
        catch (const std::out_of_range &) {
            LOG_ERROR("Application %s is not able to handle action #%d", GetName().c_str(), action);
        }
        return msgNotHandled();
    }

    sys::MessagePointer Application::handleApplicationSwitch(sys::Message *msgl)
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
            return msgHandled();
        }
        return msgNotHandled();
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
                    return msgHandled();
                }
            }
            getCurrentWindow()->onBeforeShow(msg->getCommand(), switchData.get());
            refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
        }
        else {
            LOG_ERROR("No such window: %s", msg->getWindowName().c_str());
        }
        return msgHandled();
    }

    sys::MessagePointer Application::handleAppClose(sys::Message *msgl)
    {
        setState(State::DEACTIVATING);
        app::manager::Controller::confirmClose(this);
        return msgHandled();
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
        return msgHandled();
    }

    sys::MessagePointer Application::handleAppRefresh(sys::Message *msgl)
    {
        auto *msg = static_cast<AppRefreshMessage *>(msgl);
        assert(msg);
        if (windowsStack.isEmpty() || (getCurrentWindow()->getName() != msg->getWindowName())) {
            LOG_DEBUG("Ignore request for window %s we are on window %s",
                      msg->getWindowName().c_str(),
                      windowsStack.isEmpty() ? "none" : getCurrentWindow()->getName().c_str());
            return msgNotHandled();
        }
        render(msg->getMode());
        return msgHandled();
    }

    sys::MessagePointer Application::handleAppFocusLost(sys::Message *msgl)
    {
        if (state == State::ACTIVE_FORGROUND) {
            setState(State::ACTIVE_BACKGROUND);
            app::manager::Controller::confirmSwitch(this);
        }
        return msgHandled();
    }

    sys::MessagePointer Application::handleSIMMessage(sys::Message *msgl)
    {
        getCurrentWindow()->setSIM();
        refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        return msgHandled();
    }

    sys::ReturnCodes Application::InitHandler()
    {
        setState(State::INITIALIZING);

        app::manager::Controller::applicationInitialised(this, StartupStatus::Success, startInBackground);

        if (startInBackground) {
            setState(State::ACTIVE_BACKGROUND);
        }

        settings->registerValueChange(
            settings::SystemProperties::lockScreenPasscodeIsOn,
            [this](const std::string &value) { setLockScreenPasscodeOn(utils::getNumericValue<bool>(value)); },
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

    bool Application::setVolume(const audio::Volume &value,
                                const audio::Profile::Type &profileType,
                                const audio::PlaybackType &playbackType)
    {
        const auto ret = AudioServiceAPI::SetSetting(this, audio::Setting::Volume, value, playbackType, profileType);
        return ret == audio::RetCode::Success;
    }

    auto Application::getVolume(audio::Volume &volume,
                                const audio::Profile::Type &profileType,
                                const audio::PlaybackType &playbackType)
    {
        return AudioServiceAPI::GetSetting(this, audio::Setting::Volume, volume, playbackType, profileType);
    }

    bool Application::adjustCurrentVolume(const int step)
    {
        return AudioServiceAPI::KeyPressed(this, step);
    }

    audio::RetCode Application::getCurrentVolume(audio::Volume &volume)
    {
        return AudioServiceAPI::GetSetting(this, audio::Setting::Volume, volume);
    }

    void Application::toggleTorch(bsp::torch::ColourTemperature temperature)
    {
        using namespace bsp;

        auto retGetState = sys::Bus::SendUnicast(std::make_shared<sevm::TorchStateMessage>(torch::Action::getState),
                                                 service::name::evt_manager,
                                                 this,
                                                 pdMS_TO_TICKS(1500));
        if (retGetState.first == sys::ReturnCodes::Success) {
            auto msgGetState = dynamic_cast<sevm::TorchStateResultMessage *>(retGetState.second.get());
            if (msgGetState == nullptr) {
                return;
            }
            auto msgSetState = std::make_shared<sevm::TorchStateMessage>(torch::Action::setState);

            switch (msgGetState->state) {
            case torch::State::off:
                msgSetState->state      = torch::State::on;
                msgSetState->colourTemp = temperature;
                break;
            case torch::State::on:
                msgSetState->state = torch::State::off;
                break;
            }
            sys::Bus::SendUnicast(msgSetState, service::name::evt_manager, this);
        }
    }

    void Application::requestAction(sys::Service *sender,
                                    const ApplicationName &applicationName,
                                    manager::actions::ActionId actionId,
                                    manager::actions::ActionParamsPtr &&data)
    {
        auto msg = std::make_shared<AppActionRequest>(actionId, std::move(data));
        sys::Bus::SendUnicast(msg, applicationName, sender);
    }

    void Application::messageSwitchApplication(sys::Service *sender,
                                               std::string application,
                                               std::string window,
                                               std::unique_ptr<gui::SwitchData> data)
    {
        auto msg = std::make_shared<AppSwitchMessage>(application, window, std::move(data));
        sys::Bus::SendUnicast(msg, application, sender);
    }

    void Application::messageCloseApplication(sys::Service *sender, std::string application)
    {

        auto msg = std::make_shared<AppMessage>(MessageType::AppClose);
        sys::Bus::SendUnicast(msg, application, sender);
    }

    void Application::messageRebuildApplication(sys::Service *sender, std::string application)
    {
        auto msg = std::make_shared<AppRebuildMessage>();
        sys::Bus::SendUnicast(msg, application, sender);
    }

    void Application::messageApplicationLostFocus(sys::Service *sender, std::string application)
    {
        auto msg = std::make_shared<AppLostFocusMessage>();
        sys::Bus::SendUnicast(msg, application, sender);
    }

    void Application::messageInputEventApplication(sys::Service *sender,
                                                   std::string application,
                                                   const gui::InputEvent &event)
    {
        auto msg = std::make_shared<AppInputEventMessage>(event);
        sys::Bus::SendUnicast(msg, application, sender);
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

    void Application::connect(std::unique_ptr<app::GuiTimer> &&timer, gui::Item *item)
    {
        timer->sysapi.connect(item);
        item->attachTimer(std::move(timer));
    }

    const gui::top_bar::Configuration &Application::getTopBarConfiguration() const noexcept
    {
        return topBarManager->getConfiguration();
    }

    void Application::addActionReceiver(manager::actions::ActionId actionId, OnActionReceived &&callback)
    {
        receivers.insert_or_assign(actionId, std::move(callback));
    }

    void Application::timeFormatChanged(std::string value)
    {
        timeFormat12 = utils::getNumericValue<bool>(value);
    }

    bool Application::isTimeFormat12() const noexcept
    {
        return timeFormat12;
    }

    void Application::cancelCallbacks(AsyncCallbackReceiver::Ptr receiver)
    {
        callbackStorage->removeAll(receiver);
    }

    void Application::setLockScreenPasscodeOn(bool screenPasscodeOn) noexcept
    {
        lockScreenPasscodeIsOn = screenPasscodeOn;
        settings->setValue(settings::SystemProperties::lockScreenPasscodeIsOn, std::to_string(lockScreenPasscodeIsOn));
    }

    bool Application::isLockScreenPasscodeOn() const noexcept
    {
        return lockScreenPasscodeIsOn;
    }
} /* namespace app */
