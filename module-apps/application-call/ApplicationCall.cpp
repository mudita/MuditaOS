/*
 * @file ApplicationCall.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "ApplicationCall.hpp"

#include "data/CallSwitchData.hpp"
#include "windows/CallMainWindow.hpp"
#include "windows/CallWindow.hpp"
#include "windows/EmergencyCallWindow.hpp"
#include "windows/EnterNumberWindow.hpp"

#include <Application.hpp>
#include <MessageType.hpp>
#include <UiCommonActions.hpp>

#include <log/log.hpp>
#include <service-cellular/ServiceCellular.hpp>
#include <service-cellular/api/CellularServiceAPI.hpp>
#include <service-audio/api/AudioServiceAPI.hpp>
#include <service-appmgr/ApplicationManager.hpp>
#include <time/time_conversion.hpp>
#include <ticks.hpp>

#include <cassert>

namespace app
{

    ApplicationCall::ApplicationCall(std::string name, std::string parent, bool startBackgound)
        : Application(name, parent, startBackgound, app::call_stack_size),
          timerCall(CreateAppTimer(1000, true, [=]() { timerCallCallback(); }))
    {}

    //  number of seconds after end call to switch back to previous application
    const inline utils::time::Duration delayToSwitchToPreviousApp = 3;

    void ApplicationCall::timerCallCallback()
    {
        callDuration = utils::time::Timestamp() - callStartTime;

        auto it = windows.find(window::name_call);
        if (getCurrentWindow() == it->second) {
            auto callWindow = dynamic_cast<gui::CallWindow *>(getCurrentWindow());

            if (callWindow && callWindow->getState() == gui::CallWindow::State::CALL_IN_PROGRESS) {
                callWindow->updateDuration(callDuration);
                refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
            }
        }

        // delayed switch to previous application
        if (callDelayedStopTime != 0 && utils::time::Timestamp() >= callDelayedStopTime) {
            stopCallTimer();
            sapm::ApplicationManager::messageSwitchPreviousApplication(this);
        }
    }

    void ApplicationCall::CallAbortHandler()
    {
        gui::CallWindow *callWindow = dynamic_cast<gui::CallWindow *>(windows.find(window::name_call)->second);
        assert(callWindow != nullptr);

        LOG_INFO("---------------------------------CallAborted");
        AudioServiceAPI::Stop(this);
        callDelayedStopTime = utils::time::Timestamp() + delayToSwitchToPreviousApp;
        callWindow->setState(gui::CallWindow::State::CALL_ENDED);
        refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
    }

    void ApplicationCall::CallActiveHandler()
    {
        gui::CallWindow *callWindow = dynamic_cast<gui::CallWindow *>(windows.find(window::name_call)->second);
        assert(callWindow != nullptr);

        runCallTimer();

        LOG_INFO("---------------------------------CallActive");
        callWindow->setState(gui::CallWindow::State::CALL_IN_PROGRESS);
        refreshWindow(gui::RefreshModes::GUI_REFRESH_DEEP);
    }

    void ApplicationCall::IncomingCallHandler(const CellularNotificationMessage *const msg)
    {
        gui::CallWindow *callWindow = dynamic_cast<gui::CallWindow *>(windows.find(window::name_call)->second);
        assert(callWindow != nullptr);

        LOG_INFO("---------------------------------IncomingCall");
        if (callWindow->getState() == gui::CallWindow::State::INCOMING_CALL) {
            LOG_INFO("ignoring call incoming");
        }
        else {
            AudioServiceAPI::RoutingStart(this);
            runCallTimer();
            std::unique_ptr<gui::SwitchData> data = std::make_unique<app::IncommingCallData>(msg->data);
            // send to itself message to switch (run) call application
            callWindow->setState(gui::CallWindow::State::INCOMING_CALL);
            if (getState() == State::ACTIVE_FORGROUND) {
                LOG_INFO("++++++++++++WINDOW SWITCH");
                switchWindow(window::name_call, std::move(data));
            }
            else {
                LOG_INFO("++++++++++++APP SWITCH");

                sapm::ApplicationManager::messageSwitchApplication(this, name_call, window::name_call, std::move(data));
            }
        }
    }

    void ApplicationCall::RingingHandler(const CellularNotificationMessage *const msg)
    {
        gui::CallWindow *callWindow = dynamic_cast<gui::CallWindow *>(windows.find(window::name_call)->second);
        assert(callWindow != nullptr);

        LOG_INFO("---------------------------------Ringing");
        AudioServiceAPI::RoutingStart(this);
        runCallTimer();

        std::unique_ptr<gui::SwitchData> data = std::make_unique<app::ExecuteCallData>(msg->data);
        callWindow->setState(gui::CallWindow::State::OUTGOING_CALL);
        if (getState() == State::ACTIVE_FORGROUND) {
            switchWindow(window::name_call, std::move(data));
        }
    }

    // Invoked upon receiving data message
    sys::Message_t ApplicationCall::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {

        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        if ((reinterpret_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success)) {
            return retMsg;
        }

        if (msgl->messageType == MessageType::CellularNotification) {
            CellularNotificationMessage *msg = dynamic_cast<CellularNotificationMessage *>(msgl);
            assert(msg != nullptr);

            switch (msg->type) {
            case CellularNotificationMessage::Type::CallAborted: {
                CallAbortHandler();
            } break;
            case CellularNotificationMessage::Type::CallActive: {
                CallActiveHandler();
            } break;
            case CellularNotificationMessage::Type::IncomingCall: {
                IncomingCallHandler(msg);
            } break;
            case CellularNotificationMessage::Type::Ringing: {
                RingingHandler(msg);
            } break;
            default:
                break;
            }

            return std::make_shared<sys::ResponseMessage>();
        }

        if (resp != nullptr) {
            switch (resp->responseTo) {
            case MessageType::CellularHangupCall: {
                if (resp->retCode == sys::ReturnCodes::Success) {
                    CallAbortHandler();
                }
                break;
            }
            default:
                break;
            }

            return std::make_shared<sys::ResponseMessage>();
        }

        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
    }

    // Invoked during initialization
    sys::ReturnCodes ApplicationCall::InitHandler()
    {

        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success)
            return ret;

        createUserInterface();

        setActiveWindow("MainWindow");

        return ret;
    }

    sys::ReturnCodes ApplicationCall::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    void ApplicationCall::createUserInterface()
    {
        gui::AppWindow *window = nullptr;

        window = new gui::CallMainWindow(this);
        windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

        window = new gui::EnterNumberWindow(this);
        windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

        window = new gui::CallWindow(this);
        windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));

        window = new gui::EmergencyCallWindow(this);
        windows.insert(std::pair<std::string, gui::AppWindow *>(window->getName(), window));
    }

    void ApplicationCall::runCallTimer()
    {
        callStartTime       = utils::time::Timestamp();
        callDuration        = 0;
        callDelayedStopTime = 0;
        timerCall.restart();
    }

    void ApplicationCall::stopCallTimer()
    {
        callStartTime       = 0;
        callDuration        = 0;
        callDelayedStopTime = 0;
        timerCall.stop();
    }

    void ApplicationCall::destroyUserInterface()
    {}

    void ApplicationCall::handleCallEvent(const std::string &number)
    {
        LOG_INFO("number: [%s]", number.c_str());
        auto ret = CellularServiceAPI::DialNumber(this, number.c_str());
        LOG_INFO("CALL RESULT: %s", (ret ? "OK" : "FAIL"));
    }

    void ApplicationCall::handleAddContactEvent(const std::string &number)
    {
        LOG_INFO("add contact information: %s", number.c_str());
        app::contact(this, app::ContactOperation::Add, number);
    }

} // namespace app
