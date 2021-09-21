// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotificationsHandler.hpp"
#include <service-db/DBServiceAPI.hpp>
#include <log/log.hpp>
#include <service-cellular/service-cellular/CellularMessage.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <service-cellular/CellularServiceAPI.hpp>

using namespace notifications;

NotificationsHandler::NotificationsHandler(sys::Service *parentService, NotificationsConfiguration &notifcationConfig)
    : parentService{parentService}, notifcationConfig{notifcationConfig}
{}

void NotificationsHandler::registerMessageHandlers()
{
    parentService->connect(typeid(CellularIncominCallMessage), [&](sys::Message *request) -> sys::MessagePointer {
        incomingCallHandler(request);
        return sys::msgHandled();
    });

    parentService->connect(typeid(CellularCallerIdMessage), [&](sys::Message *request) -> sys::MessagePointer {
        callerIdHandler(request);
        return sys::msgHandled();
    });

    parentService->connect(typeid(CellularIncomingSMSNotificationMessage),
                           [&](sys::Message *request) -> sys::MessagePointer {
                               incomingSMSHandler();
                               return sys::msgHandled();
                           });
}

void NotificationsHandler::incomingCallHandler(sys::Message *request)
{
    notifcationConfig.updateCurrentCall(currentCallPolicy);

    if (currentCallPolicy.isPopupAllowed()) {
        auto msg = static_cast<CellularIncominCallMessage *>(request);
        app::manager::Controller::sendAction(parentService,
                                             app::manager::actions::HandleIncomingCall,
                                             std::make_unique<app::manager::actions::CallParams>(msg->number));
        playbackCallRingtone();
    }
}

void NotificationsHandler::callerIdHandler(sys::Message *request)
{
    auto msg = static_cast<CellularCallerIdMessage *>(request);

    if (currentCallPolicy.isNumberCheckRequired()) {
        policyNumberCheck(msg->number);
    }
    if (currentCallPolicy.isPopupAllowed()) {
        app::manager::Controller::sendAction(parentService,
                                             app::manager::actions::HandleCallerId,
                                             std::make_unique<app::manager::actions::CallParams>(msg->number));
        playbackCallRingtone();
    }
    else {
        CellularServiceAPI::DismissCall(parentService, currentCallPolicy.isDismissedCallNotificationAllowed());
    }
}

void NotificationsHandler::policyNumberCheck(const utils::PhoneNumber::View &number)
{
    auto isContactInFavourites = DBServiceAPI::IsContactInFavourites(parentService, number);
    notifcationConfig.callNumberCheck(currentCallPolicy, isContactInFavourites);
}

void NotificationsHandler::incomingSMSHandler()
{
    notifcationConfig.updateCurrentSMS(currentSMSPolicy);
    playbackSMSRingtone();
}

void NotificationsHandler::playbackCallRingtone()
{
    if (currentCallPolicy.isRingtoneAllowed()) {
        const auto filePath = AudioServiceAPI::GetSound(parentService, audio::PlaybackType::CallRingtone);
        AudioServiceAPI::PlaybackStart(parentService, audio::PlaybackType::CallRingtone, filePath);
    }
}

void NotificationsHandler::playbackSMSRingtone()
{
    if (currentSMSPolicy.isRingtoneAllowed()) {
        const auto filePath = AudioServiceAPI::GetSound(parentService, audio::PlaybackType::TextMessageRingtone);
        AudioServiceAPI::PlaybackStart(parentService, audio::PlaybackType::TextMessageRingtone, filePath);
    }
}
