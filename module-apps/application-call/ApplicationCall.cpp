// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationCall.hpp"
#include "CallSwitchData.hpp"
#include "CallWindow.hpp"
#include "EmergencyCallWindow.hpp"
#include "EnterNumberWindow.hpp"

#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <apps-common/windows/Dialog.hpp>
#include <apps-common/windows/DialogMetadata.hpp>
#include <log/log.hpp>
#include <module-apps/application-phonebook/data/PhonebookItemData.hpp>
#include <Timers/TimerFactory.hpp>
#include <PhoneNumber.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-appmgr/data/MmiActionsParams.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <service-cellular/CellularServiceAPI.hpp>
#include <time/time_conversion.hpp>

#include <cassert>
#include <memory>

namespace app
{
    ApplicationCall::ApplicationCall(std::string name,
                                     std::string parent,
                                     StatusIndicators statusIndicators,
                                     StartInBackground startInBackground)
        : Application(name, parent, statusIndicators, startInBackground, app::call_stack_size)
    {
        using namespace gui::status_bar;
        statusBarManager->enableIndicators(
            {Indicator::Signal, Indicator::Time, Indicator::Battery, Indicator::SimCard});
        addActionReceiver(manager::actions::Call, [this](auto &&data) {
            if (auto msg = dynamic_cast<app::CallSwitchData *>(data.get()); msg != nullptr) {
                handleCallEvent(msg->getPhoneNumber().getEntered());
                return actionHandled();
            }
            return actionNotHandled();
        });
        addActionReceiver(manager::actions::Dial, [this](auto &&data) {
            switchWindow(window::name_enterNumber, std::forward<decltype(data)>(data));
            return actionHandled();
        });
        addActionReceiver(manager::actions::EmergencyDial, [this](auto &&data) {
            switchWindow(app::window::name_emergencyCall, std::forward<decltype(data)>(data));
            return actionHandled();
        });
        addActionReceiver(manager::actions::NotAnEmergencyNotification, [this](auto &&data) {
            auto buttonAction = [=]() -> bool {
                returnToPreviousWindow();
                return true;
            };
            constexpr auto iconNoEmergency = "fail_128px_W_G";
            auto textNoEmergency           = utils::translate("app_call_wrong_emergency");
            utils::findAndReplaceAll(textNoEmergency, "$NUMBER", data->getDescription());
            showNotification(buttonAction, iconNoEmergency, textNoEmergency);
            return actionHandled();
        });
        addActionReceiver(manager::actions::NoSimNotification, [this](auto &&data) {
            auto buttonAction = [=]() -> bool {
                returnToPreviousWindow();
                return true;
            };
            constexpr auto iconNoSim = "info_128px_W_G";
            const auto textNoSim     = utils::translate("app_call_no_sim");
            showNotification(buttonAction, iconNoSim, textNoSim);
            return actionHandled();
        });
        addActionReceiver(manager::actions::CallRejectedByOfflineNotification, [this](auto &&data) {
            auto buttonAction = [=]() -> bool {
                app::manager::Controller::switchBack(this);
                return true;
            };
            constexpr auto icon    = "info_128px_W_G";
            const auto textOffline = utils::translate("app_call_offline");
            showNotification(buttonAction, icon, textOffline);
            return actionHandled();
        });
        addActionReceiver(manager::actions::AbortCall, [this](auto &&data) {
            callerIdTimer.reset();
            if (const auto state = getState(); state == Application::State::ACTIVE_FORGROUND) {
                switchWindow(window::name_call, std::make_unique<app::CallAbortData>());
            }
            else if (state == Application::State::ACTIVE_BACKGROUND) {
                stopAudio();
                setCallState(call::State::IDLE);
                manager::Controller::finish(this);
            }
            return actionHandled();
        });
        addActionReceiver(manager::actions::ActivateCall, [this](auto &&data) {
            switchWindow(window::name_call, std::make_unique<app::CallActiveData>());
            return actionHandled();
        });
        addActionReceiver(manager::actions::HandleOutgoingCall, [this](auto &&data) {
            auto callParams = static_cast<app::manager::actions::CallParams *>(data.get());
            switchWindow(window::name_call, std::make_unique<app::ExecuteCallData>(callParams->getNumber()));
            return actionHandled();
        });
        addActionReceiver(manager::actions::HandleIncomingCall, [this](auto &&data) {
            handleIncomingCall();
            return actionHandled();
        });
        addActionReceiver(manager::actions::HandleCallerId, [this](auto &&data) {
            auto callParams = static_cast<app::manager::actions::CallParams *>(data.get());
            handleCallerId(callParams);
            return actionHandled();
        });
    }

    bool ApplicationCall::isPopupPermitted(gui::popup::ID popupId) const
    {
        if (popupId == gui::popup::ID::Volume) {
            return true;
        }
        return getCallState() == call::State::IDLE;
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
        return handleAsyncResponse(resp);
    } // namespace app

    // Invoked during initialization
    sys::ReturnCodes ApplicationCall::InitHandler()
    {

        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        createUserInterface();

        return ret;
    }

    void ApplicationCall::createUserInterface()
    {
        windowsFactory.attach(app::window::name_enterNumber, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::EnterNumberWindow>(app, static_cast<ApplicationCall *>(app));
        });
        windowsFactory.attach(app::window::name_call, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::CallWindow>(app, static_cast<ApplicationCall *>(app));
        });
        windowsFactory.attach(app::window::name_emergencyCall, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::EmergencyCallWindow>(app, static_cast<ApplicationCall *>(app));
        });
        windowsFactory.attach(app::window::name_dialogConfirm, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::DialogConfirm>(app, name);
        });
        attachPopups({gui::popup::ID::Volume, gui::popup::ID::Tethering, gui::popup::ID::PhoneModes});
    }

    bool ApplicationCall::showNotification(std::function<bool()> action,
                                           const std::string &icon,
                                           const std::string &text)
    {
        auto metaData =
            std::make_unique<gui::DialogMetadataMessage>(gui::DialogMetadata{"", icon, text, "", std::move(action)});
        switchWindow(app::window::name_dialogConfirm, gui::ShowMode::GUI_SHOW_INIT, std::move(metaData));
        return true;
    }

    void ApplicationCall::destroyUserInterface()
    {}

    void ApplicationCall::handleIncomingCall()
    {
        if (getCallState() != call::State::IDLE) {
            return;
        }

        constexpr auto callerIdTimeout = std::chrono::seconds{1};
        callerIdTimer                  = sys::TimerFactory::createSingleShotTimer(
            this, "CallerIdTimer", callerIdTimeout, [this]([[maybe_unused]] sys::Timer &timer) {
                switchWindow(window::name_call,
                             std::make_unique<app::IncomingCallData>(utils::PhoneNumber().getView()));
            });
        callerIdTimer.start();
    }

    void ApplicationCall::handleCallerId(const app::manager::actions::CallParams *params)
    {
        if (getCallState() != call::State::IDLE) {
            return;
        }
        if (callerIdTimer.isValid()) {
            callerIdTimer.stop();
            callerIdTimer.reset();
        }
        switchWindow(window::name_call, std::make_unique<app::IncomingCallData>(params->getNumber()));
    }

    void ApplicationCall::handleEmergencyCallEvent(const std::string &number)
    {
        CellularServiceAPI::DialEmergencyNumber(this, utils::PhoneNumber(number));
    }

    void ApplicationCall::handleCallEvent(const std::string &number)
    {
        CellularServiceAPI::DialNumber(this, utils::PhoneNumber(number));
    }

    void ApplicationCall::handleAddContactEvent(const std::string &number)
    {
        LOG_INFO("add contact information");

        auto numberView    = utils::PhoneNumber(number).getView();
        auto searchResults = DBServiceAPI::MatchContactByPhoneNumber(this, numberView);
        if (searchResults != nullptr) {
            LOG_INFO("Found contact matching search num : contact ID %" PRIu32, searchResults->ID);
            app::manager::Controller::sendAction(this,
                                                 app::manager::actions::EditContact,
                                                 std::make_unique<PhonebookItemData>(std::move(searchResults)));
        }
        else {
            auto contactRecord = std::make_shared<ContactRecord>();
            contactRecord->numbers.emplace_back(std::move(numberView));

            auto data                        = std::make_unique<PhonebookItemData>(std::move(contactRecord));
            data->ignoreCurrentWindowOnStack = true;
            app::manager::Controller::sendAction(
                this, manager::actions::AddContact, std::move(data), manager::OnSwitchBehaviour::RunInBackground);
        }
    }

    void ApplicationCall::stopAudio()
    {
        AudioServiceAPI::StopAll(this);
    }

    void ApplicationCall::startAudioRouting()
    {
        AudioServiceAPI::RoutingStart(this);
    }

    void ApplicationCall::sendAudioEvent(AudioEvent audioEvent)
    {
        switch (audioEvent) {
        case AudioEvent::Mute:
            AudioServiceAPI::SendEvent(this, audio::EventType::CallMute);
            break;
        case AudioEvent::Unmute:
            AudioServiceAPI::SendEvent(this, audio::EventType::CallUnmute);
            break;
        case AudioEvent::LoudspeakerOn:
            AudioServiceAPI::SendEvent(this, audio::EventType::CallLoudspeakerOn);
            break;
        case AudioEvent::LoudspeakerOff:
            AudioServiceAPI::SendEvent(this, audio::EventType::CallLoudspeakerOff);
            break;
        }
    }

    void ApplicationCall::hangupCall()
    {
        CellularServiceAPI::HangupCall(this);
    }

    void ApplicationCall::answerIncomingCall()
    {
        CellularServiceAPI::AnswerIncomingCall(this);
    }

    void ApplicationCall::transmitDtmfTone(uint32_t digit)
    {
        if (!CellularServiceAPI::TransmitDtmfTones(this, digit)) {
            LOG_ERROR("transmitDtmfTone failed");
        }
    }
} // namespace app
