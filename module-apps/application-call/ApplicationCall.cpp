// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationCall.hpp"

#include "DialogMetadata.hpp"
#include "DialogMetadataMessage.hpp"
#include "application-call/data/CallWindowData.hpp"
#include "data/CallSwitchData.hpp"
#include "windows/CallMainWindow.hpp"
#include "windows/CallWindow.hpp"
#include "windows/EmergencyCallWindow.hpp"
#include "windows/EnterNumberWindow.hpp"

#include <Application.hpp>
#include <MessageType.hpp>
#include <PhoneNumber.hpp>
#include <UiCommonActions.hpp>
#include <Dialog.hpp>
#include <log/log.hpp>
#include <memory>
#include <service-cellular/ServiceCellular.hpp>
#include <service-cellular/api/CellularServiceAPI.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <service-appmgr/Controller.hpp>
#include <time/time_conversion.hpp>
#include <ticks.hpp>

#include <cassert>

namespace app
{
    ApplicationCall::ApplicationCall(std::string name, std::string parent, StartInBackground startInBackground)
        : Application(name, parent, startInBackground, app::call_stack_size)
    {
        addActionReceiver(manager::actions::Call, [this](auto data) {
            switchWindow(window::name_enterNumber, std::move(data));
            return msgHandled();
        });
    }

    //  number of seconds after end call to switch back to previous application
    const inline utils::time::Duration delayToSwitchToPreviousApp = 3;

    void switchWindowOrApp(Application *app, std::unique_ptr<gui::SwitchData> &&data)
    {
        if (app->getState() == Application::State::ACTIVE_FORGROUND) {
            app->switchWindow(window::name_call, std::move(data));
        }
        else {
            app::manager::Controller::switchApplication(app, name_call, window::name_call, std::move(data));
        }
    }

    void ApplicationCall::CallAbortHandler()
    {
        switchWindowOrApp(this, std::make_unique<app::CallAbortData>());
    }

    void ApplicationCall::CallActiveHandler()
    {
        switchWindowOrApp(this, std::make_unique<app::CallActiveData>());
    }

    void ApplicationCall::IncomingCallHandler(const CellularCallMessage *const msg)
    {
        switchWindowOrApp(this, std::make_unique<app::IncomingCallData>(msg->number));
    }

    void ApplicationCall::RingingHandler(const CellularCallMessage *const msg)
    {
        switchWindowOrApp(this, std::make_unique<app::ExecuteCallData>(msg->number));
    }

    // Invoked upon receiving data message
    sys::MessagePointer ApplicationCall::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
    {

        auto retMsg = Application::DataReceivedHandler(msgl);
        // if message was handled by application's template there is no need to process further.
        auto response = dynamic_cast<sys::ResponseMessage *>(retMsg.get());
        assert(response);
        if (response->retCode == sys::ReturnCodes::Success) {
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
            default:
                break;
            }

            return std::make_shared<sys::ResponseMessage>();
        }
        else if (msgl->messageType == MessageType::CellularCall) {
            auto *msg = dynamic_cast<CellularCallMessage *>(msgl);
            assert(msg != nullptr);

            switch (msg->type) {
            case CellularCallMessage::Type::Ringing: {
                RingingHandler(msg);
            } break;
            case CellularCallMessage::Type::IncomingCall: {
                IncomingCallHandler(msg);
            } break;
            }
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
        windowsFactory.attach(gui::name::window::main_window, [](Application *app, const std::string name) {
            return std::make_unique<gui::CallMainWindow>(app);
        });
        windowsFactory.attach(app::window::name_enterNumber, [](Application *app, const std::string newname) {
            return std::make_unique<gui::EnterNumberWindow>(app);
        });
        windowsFactory.attach(app::window::name_call, [](Application *app, const std::string &name) {
            return std::make_unique<gui::CallWindow>(app);
        });
        windowsFactory.attach(app::window::name_emergencyCall, [](Application *app, const std::string &name) {
            return std::make_unique<gui::EmergencyCallWindow>(app);
        });
        windowsFactory.attach(app::window::name_dialogConfirm, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DialogConfirm>(app, name);
        });
    }

    bool ApplicationCall::showNotification(std::function<bool()> action)
    {
        gui::DialogMetadata meta;
        meta.icon   = "info_big_circle_W_G";
        meta.text   = utils::localize.get("app_call_no_sim");
        meta.action = action;
        switchWindow(app::window::name_dialogConfirm, std::make_unique<gui::DialogMetadataMessage>(meta));
        return true;
    }

    void ApplicationCall::destroyUserInterface()
    {}

    void ApplicationCall::handleCallEvent(const std::string &number)
    {
        if (!Store::GSM::get()->simCardInserted()) {
            LOG_INFO("No SIM card");
            auto action = [=]() -> bool {
                returnToPreviousWindow();
                return true;
            };
            showNotification(action);
            return;
        }

        LOG_INFO("number: [%s]", number.c_str());
        auto ret = CellularServiceAPI::DialNumber(this, utils::PhoneNumber(number));
        LOG_INFO("CALL RESULT: %s", (ret ? "OK" : "FAIL"));
    }

    void ApplicationCall::handleAddContactEvent(const std::string &number)
    {
        LOG_INFO("add contact information: %s", number.c_str());
        app::contact(this, app::ContactOperation::Add, number);
    }

    void ApplicationCall::transmitDtmfTone(uint32_t digit)
    {
        if (!CellularServiceAPI::TransmitDtmfTones(this, digit)) {
            LOG_ERROR("transmitDtmfTone failed");
        }
    }

    void ApplicationCall::stopAudio()
    {
        AudioServiceAPI::StopAll(this);
    }

    void ApplicationCall::startRinging()
    {
        AudioServiceAPI::PlaybackStart(this, audio::PlaybackType::CallRingtone, ringtone_path);
    }

    void ApplicationCall::startRouting()
    {
        AudioServiceAPI::RoutingStart(this);
    }
} // namespace app
