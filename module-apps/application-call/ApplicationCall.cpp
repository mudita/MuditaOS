﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationCall.hpp"

#include "DialogMetadata.hpp"
#include "DialogMetadataMessage.hpp"
#include "data/CallSwitchData.hpp"
#include "windows/CallWindow.hpp"
#include "windows/EmergencyCallWindow.hpp"
#include "windows/EnterNumberWindow.hpp"

#include <Application.hpp>
#include <MessageType.hpp>
#include <PhoneNumber.hpp>
#include <Dialog.hpp>
#include <log/log.hpp>
#include <memory>
#include <service-cellular/ServiceCellular.hpp>
#include <service-cellular/CellularServiceAPI.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <service-appmgr/Controller.hpp>
#include <time/time_conversion.hpp>
#include <ticks.hpp>

#include <cassert>
#include <module-apps/application-phonebook/data/PhonebookItemData.hpp>
#include <module-services/service-db/service-db/DBServiceAPI.hpp>
#include <module-sys/Timers/TimerFactory.hpp>

namespace app
{
    ApplicationCall::ApplicationCall(std::string name,
                                     std::string parent,
                                     sys::phone_modes::PhoneMode mode,
                                     StartInBackground startInBackground)
        : Application(name, parent, mode, startInBackground, app::call_stack_size)
    {
        using namespace gui::top_bar;
        topBarManager->enableIndicators({Indicator::Signal,
                                         Indicator::Time,
                                         Indicator::Battery,
                                         Indicator::SimCard,
                                         Indicator::NetworkAccessTechnology});
        addActionReceiver(manager::actions::Call, [this](auto &&data) {
            switchWindow(window::name_call, std::forward<decltype(data)>(data));
            return actionHandled();
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
            constexpr auto iconNoEmergency = "emergency_W_G";
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
            constexpr auto iconNoSim = "info_big_circle_W_G";
            const auto textNoSim     = utils::translate("app_call_no_sim");
            showNotification(buttonAction, iconNoSim, textNoSim);
            return actionHandled();
        });
        addActionReceiver(manager::actions::CallRejectedByOfflineNotification, [this](auto &&data) {
            auto buttonAction = [=]() -> bool {
                returnToPreviousWindow();
                return true;
            };
            constexpr auto icon    = "info_big_circle_W_G";
            const auto textOffline = utils::translate("app_call_offline");
            showNotification(buttonAction, icon, textOffline);
            return actionHandled();
        });
    }

    bool ApplicationCall::isPopupPermitted(gui::popup::ID popupId) const
    {
        if (popupId == gui::popup::ID::Volume) {
            return true;
        }
        return getState() == call::State::IDLE;
    }

    //  number of seconds after end call to switch back to previous application
    const inline utils::time::Duration delayToSwitchToPreviousApp = 3;

    void ApplicationCall::CallAbortHandler()
    {
        manager::Controller::sendAction(this, manager::actions::Call, std::make_unique<app::CallAbortData>());
    }

    void ApplicationCall::CallActiveHandler()
    {
        manager::Controller::sendAction(this, manager::actions::Call, std::make_unique<app::CallActiveData>());
    }

    void ApplicationCall::CallerIdHandler(const CellularCallerIdMessage *const msg)
    {
        if (getState() == call::State::IDLE) {
            if (callerIdTimer.isValid()) {
                callerIdTimer.stop();
                callerIdTimer.reset();
            }
            manager::Controller::sendAction(
                this, manager::actions::Call, std::make_unique<app::IncomingCallData>(msg->number));
        }
    }

    void ApplicationCall::IncomingCallHandler(const CellularIncominCallMessage *const msg)
    {
        if (getState() == call::State::IDLE) {
            constexpr auto callerIdTimeout = std::chrono::milliseconds{1000};
            callerIdTimer =
                sys::TimerFactory::createSingleShotTimer(this, "CallerIdTimer", callerIdTimeout, [=](sys::Timer &) {
                    manager::Controller::sendAction(
                        this,
                        manager::actions::Call,
                        std::make_unique<app::IncomingCallData>(utils::PhoneNumber().getView()));
                });
            callerIdTimer.start();
        }
    }

    void ApplicationCall::RingingHandler(const CellularRingingMessage *const msg)
    {
        manager::Controller::sendAction(
            this, manager::actions::Call, std::make_unique<app::ExecuteCallData>(msg->number));
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

        if (auto msg = dynamic_cast<CellularMessage *>(msgl)) {
            if (msg->type == CellularMessage::Type::Notification) {
                auto *msg = dynamic_cast<CellularNotificationMessage *>(msgl);
                assert(msg != nullptr);

                switch (msg->content) {
                case CellularNotificationMessage::Content::CallAborted: {
                    callerIdTimer.reset();
                    CallAbortHandler();
                } break;
                case CellularNotificationMessage::Content::CallActive: {
                    CallActiveHandler();
                } break;
                default:
                    break;
                }

                return std::make_shared<sys::ResponseMessage>();
            }
            else if (msg->type == CellularMessage::Type::Ringing) {
                auto msg = dynamic_cast<CellularRingingMessage *>(msgl);
                assert(msg != nullptr);
                RingingHandler(msg);
            }
            else if (msg->type == CellularMessage::Type::IncomingCall) {
                auto msg = dynamic_cast<CellularIncominCallMessage *>(msgl);
                assert(msg != nullptr);
                IncomingCallHandler(msg);
            }
            else if (msg->type == CellularMessage::Type::CallerId) {
                auto msg = dynamic_cast<CellularCallerIdMessage *>(msgl);
                assert(msg != nullptr);
                CallerIdHandler(msg);
            }
        }

        if (auto msg = dynamic_cast<CellularResponseMessage *>(resp)) {
            if (msg->cellResponse == CellularMessage::Type::HangupCall) {
                if (resp->retCode == sys::ReturnCodes::Success) {
                    CallAbortHandler();
                }
            }
            return std::make_shared<sys::ResponseMessage>();
        }

        return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
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

    sys::ReturnCodes ApplicationCall::DeinitHandler()
    {
        return sys::ReturnCodes::Success;
    }

    void ApplicationCall::createUserInterface()
    {
        windowsFactory.attach(app::window::name_enterNumber, [](Application *app, const std::string newname) {
            return std::make_unique<gui::EnterNumberWindow>(app, static_cast<ApplicationCall *>(app));
        });
        windowsFactory.attach(app::window::name_call, [](Application *app, const std::string &name) {
            return std::make_unique<gui::CallWindow>(app, static_cast<ApplicationCall *>(app));
        });
        windowsFactory.attach(app::window::name_emergencyCall, [](Application *app, const std::string &name) {
            return std::make_unique<gui::EmergencyCallWindow>(app, static_cast<ApplicationCall *>(app));
        });
        windowsFactory.attach(app::window::name_dialogConfirm, [](Application *app, const std::string &name) {
            return std::make_unique<gui::DialogConfirm>(app, name);
        });
        attachPopups({gui::popup::ID::Volume,
                      gui::popup::ID::Tethering,
                      gui::popup::ID::PhoneModes});
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
        LOG_INFO("add contact information: %s", number.c_str());

        auto numberView    = utils::PhoneNumber(number).getView();
        auto searchResults = DBServiceAPI::MatchContactByPhoneNumber(this, numberView);
        if (searchResults != nullptr) {
            LOG_INFO("Found contact matching search num %s : contact ID %" PRIu32 " - %s %s",
                     number.c_str(),
                     searchResults->ID,
                     searchResults->primaryName.c_str(),
                     searchResults->alternativeName.c_str());
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

    void ApplicationCall::startAudioRinging()
    {
        const auto filePath = AudioServiceAPI::GetSound(this, audio::PlaybackType::CallRingtone);
        AudioServiceAPI::PlaybackStart(this, audio::PlaybackType::CallRingtone, filePath);
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
