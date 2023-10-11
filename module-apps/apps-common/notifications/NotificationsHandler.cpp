// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotificationsHandler.hpp"
#include <service-cellular/service-cellular/CellularMessage.hpp>
#include <service-audio/AudioServiceAPI.hpp>

using namespace notifications;

NotificationsHandler::NotificationsHandler(sys::Service *parentService, NotificationsConfiguration &notificationConfig)
    : parentService{parentService}, notificationConfig{notificationConfig}
{}

void NotificationsHandler::registerMessageHandlers()
{
    parentService->connect(typeid(cellular::IncomingSMSNotificationMessage),
                           [&]([[maybe_unused]] sys::Message *request) -> sys::MessagePointer {
                               incomingSMSHandler();
                               return sys::msgHandled();
                           });
}

void NotificationsHandler::incomingSMSHandler()
{
    notificationConfig.updateCurrentSMS(currentSMSPolicy);
    playbackSMSRingtone();
}

void NotificationsHandler::playbackSMSRingtone()
{
    if (currentSMSPolicy.isRingtoneAllowed()) {
        const auto filePath = AudioServiceAPI::GetSound(parentService, audio::PlaybackType::TextMessageRingtone);
        AudioServiceAPI::PlaybackStart(parentService, audio::PlaybackType::TextMessageRingtone, filePath);
    }
}
