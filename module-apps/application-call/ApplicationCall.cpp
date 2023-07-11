﻿// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationCall.hpp"
#include "CallSwitchData.hpp"
#include "CallWindow.hpp"
#include "EmergencyCallWindow.hpp"
#include "EnterNumberWindow.hpp"
#include "presenter/CallPresenter.hpp"

#include <apps-common/messages/DialogMetadataMessage.hpp>
#include <apps-common/windows/Dialog.hpp>
#include <apps-common/windows/DialogMetadata.hpp>
#include <apps-common/WindowsStack.hpp>
#include <log/log.hpp>
#include <module-apps/application-phonebook/data/PhonebookItemData.hpp>

#include <PhoneNumber.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-appmgr/data/MmiActionsParams.hpp>
#include <service-cellular/CellularServiceAPI.hpp>
#include <time/time_conversion.hpp>
#include <WindowsPopupFilter.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <service-db/Settings.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>

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

        bus.channels.push_back(sys::BusChannel::ServiceAudioNotifications);

        getPopupFilter().addAppDependentFilter([&](const gui::PopupRequestParams &popupParams) {
            if (popupParams.getPopupId() == gui::popup::ID::Volume) {
                return true;
            }
            return true;
        });
        statusBarManager->enableIndicators(
            {Indicator::Signal, Indicator::Time, Indicator::Battery, Indicator::SimCard});

        addActionReceiver(manager::actions::Call, [this](auto &&data) {
            if (auto msg = dynamic_cast<app::CallSwitchData *>(data.get()); msg != nullptr) {
                handleCallEvent(msg->getPhoneNumber().getEntered(), ExternalRequest::True);
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
            auto textNoEmergency = utils::translate("app_call_wrong_emergency");
            utils::findAndReplaceAll(textNoEmergency, "$NUMBER", data->getDescription());
            showNotificationAndRestartCallFlow(NotificationType::Info, textNoEmergency);
            return actionHandled();
        });

        addActionReceiver(manager::actions::NoSimNotification, [this](auto &&data) {
            showNotificationAndRestartCallFlow(NotificationType::Info, utils::translate("app_call_no_sim"));
            return actionHandled();
        });
        addActionReceiver(manager::actions::NoNetworkConnectionNotification, [this](auto &&data) {
            showNotificationAndRestartCallFlow(NotificationType::Info,
                                               utils::translate("app_call_no_network_connection"));
            return actionHandled();
        });
        addActionReceiver(manager::actions::CallRequestGeneralErrorNotification, [this](auto &&data) {
            showNotificationAndRestartCallFlow(NotificationType::Failure,
                                               utils::translate("app_call_call_request_failed"));
            return actionHandled();
        });
        addActionReceiver(manager::actions::CallRejectedByOfflineNotification, [this](auto &&data) {
            showNotificationAndRestartCallFlow(NotificationType::Info, utils::translate("app_call_offline"));
            return actionHandled();
        });
        addActionReceiver(manager::actions::ActivateCall, [this](auto &&data) {
            switchWindow(window::name_call);
            return actionHandled();
        });
        addActionReceiver(manager::actions::HandleOutgoingCall, [this](auto &&data) {
            switchWindow(window::name_call);
            return actionHandled();
        });
        addActionReceiver(manager::actions::HandleIncomingCall, [this](auto &&data) {
            callModel->setState(call::CallState::Incoming);
            auto window = getCurrentWindow();
            if (window->getName() != app::window::name_call) {
                LOG_INFO("Switch to call window");
                switchWindow(app::window::name_call);
            }
            return actionHandled();
        });
        addActionReceiver(manager::actions::HandleCallerId, [this](auto &&data) {
            auto callParams = static_cast<app::manager::actions::CallParams *>(data.get());
            callModel->setPhoneNumber(callParams->getNumber());
            callModel->setState(call::CallState::Incoming);
            auto window = getCurrentWindow();
            if (window->getName() != app::window::name_call) {
                LOG_INFO("Switch to call window");
                switchWindow(app::window::name_call);
            }
            return actionHandled();
        });

        callModel     = std::make_shared<app::call::CallModel>(this);
        callPresenter = std::make_unique<call::CallWindowContract::Presenter>(this->callModel);
    }

    bool ApplicationCall::conditionalReturnToPreviousView()
    {
        // if external request simply return to previous app
        if (externalRequest == ExternalRequest::True) {
            app::manager::Controller::switchBack(this);
            return true;
        }

        // Onboarding app should not have access to main screen so if onboarding is in progress, we pop the main window
        // from the stack
        if (!utils::getNumericValue<bool>(
                settings->getValue(settings::SystemProperties::onboardingDone, settings::SettingsScope::Global))) {
            ApplicationCommon::popWindow(gui::name::window::main_window);
        }

        returnToPreviousWindow();
        return true;
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
    }

    // Invoked during initialization
    sys::ReturnCodes ApplicationCall::InitHandler()
    {

        auto ret = Application::InitHandler();
        if (ret != sys::ReturnCodes::Success) {
            return ret;
        }

        connect(typeid(cellular::CallDurationNotification), [&](sys::Message *request) {
            auto message = static_cast<cellular::CallDurationNotification *>(request);
            callModel->setTime(message->callDuration);
            return sys::MessageNone{};
        });

        connect(typeid(cellular::CallActiveNotification), [&](sys::Message *request) {
            callModel->setState(app::call::CallState::Active);
            return sys::MessageNone{};
        });

        connect(typeid(cellular::CallMissedNotification), [&](sys::Message *request) {
            callModel->setState(app::call::CallState::Missed);
            app::manager::Controller::switchBack(this);
            return sys::MessageNone{};
        });

        connect(typeid(cellular::CallEndedNotification), [&](sys::Message *request) {
            callModel->setState(app::call::CallState::Ended);
            switchWindow(app::window::name_call);
            return sys::MessageNone{};
        });

        connect(typeid(cellular::CallStartedNotification), [&](sys::Message *request) {
            auto message = static_cast<cellular::CallStartedNotification *>(request);
            callModel->setPhoneNumber(message->getNumber());
            callModel->setState(app::call::CallState::Outgoing);
            switchWindow(app::window::name_call);
            return sys::MessageNone{};
        });

        connect(typeid(AudioRoutingNotification), [this](sys::Message *request) {
            auto message = static_cast<AudioRoutingNotification *>(request);
            return handleRoutingNotification(message);
        });

        createUserInterface();

        return ret;
    }

    sys::MessagePointer ApplicationCall::handleAppClose(sys::Message *msgl)
    {
        callModel->hangUpCall();
        return ApplicationCommon::handleAppClose(msgl);
    }

    void ApplicationCall::createUserInterface()
    {
        windowsFactory.attach(app::window::name_enterNumber, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::EnterNumberWindow>(app, static_cast<ApplicationCall *>(app));
        });
        windowsFactory.attach(app::window::name_call, [this](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::CallWindow>(app, *callPresenter);
        });
        windowsFactory.attach(app::window::name_emergencyCall, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::EmergencyCallWindow>(app, static_cast<ApplicationCall *>(app));
        });
        windowsFactory.attach(app::window::name_dialogConfirm, [](ApplicationCommon *app, const std::string &name) {
            return std::make_unique<gui::DialogConfirm>(app, name);
        });
        attachPopups({gui::popup::ID::Volume,
                      gui::popup::ID::Tethering,
                      gui::popup::ID::PhoneModes,
                      gui::popup::ID::SimLock,
                      gui::popup::ID::Alarm}); // Alarm pop-up is blocked during phone call - logic is on Alarm side
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

    auto ApplicationCall::showNotificationAndRestartCallFlow(NotificationType type, const std::string &text) -> bool
    {
        auto buttonAction = [=]() -> bool { return conditionalReturnToPreviousView(); };
        auto icon         = type == NotificationType::Info ? "info_128px_W_G" : "fail_128px_W_G";
        callModel->clear();
        return showNotification(buttonAction, icon, text);
    }

    void ApplicationCall::destroyUserInterface()
    {}

    void ApplicationCall::handleEmergencyCallEvent(const std::string &number)
    {
        auto state = callModel->getState();
        if (state != call::CallState::None) {
            LOG_WARN("Cannot call in %s state", c_str(state));
            return;
        }

        if (DBServiceAPI::IsContactInEmergency(this, utils::PhoneNumber(number).getView())) {
            CellularServiceAPI::DialNumber(this, utils::PhoneNumber(number));
            return;
        }

        CellularServiceAPI::DialEmergencyNumber(this, utils::PhoneNumber(number));
    }

    void ApplicationCall::handleCallEvent(const std::string &number, ExternalRequest isExternalRequest)
    {
        auto state = callModel->getState();
        if (state != call::CallState::None) {
            LOG_WARN("Cannot call in %s state", c_str(state));
            return;
        }
        CellularServiceAPI::DialNumber(this, utils::PhoneNumber(number));
        externalRequest = isExternalRequest;
    }

    void ApplicationCall::handleAddContactEvent(const std::string &number)
    {
        LOG_INFO("add contact information");

        auto numberView    = utils::PhoneNumber(number).getView();
        auto searchResults = DBServiceAPI::MatchContactByPhoneNumber(this, numberView);
        if (searchResults != nullptr && !searchResults->isTemporary()) {
            LOG_INFO("Found contact matching (non temporary) search num : contact ID %" PRIu32, searchResults->ID);
            auto data                     = std::make_unique<PhonebookItemData>(std::move(searchResults));
            data->nameOfSenderApplication = GetName();
            app::manager::Controller::sendAction(this, app::manager::actions::EditContact, std::move(data));
        }
        else {
            auto contactRecord = std::make_shared<ContactRecord>();
            contactRecord->numbers.emplace_back(std::move(numberView));

            auto data                        = std::make_unique<PhonebookItemData>(std::move(contactRecord));
            data->nameOfSenderApplication    = GetName();
            data->ignoreCurrentWindowOnStack = true;
            app::manager::Controller::sendAction(
                this, manager::actions::AddContact, std::move(data), manager::OnSwitchBehaviour::RunInBackground);
        }
    }

    sys::MessagePointer ApplicationCall::handleRoutingNotification(AudioRoutingNotification *message)
    {
        if (auto window = getCurrentWindow(); window->getName() == app::window::name_call) {
            const auto currentRouting = message->profileType;
            callPresenter->processCurrentRouting(currentRouting);
        }
        return sys::MessageNone{};
    }

} // namespace app
