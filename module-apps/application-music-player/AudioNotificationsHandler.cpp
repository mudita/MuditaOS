// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AudioNotificationsHandler.hpp"

#include <service-audio/AudioMessage.hpp>

namespace app::music_player
{

    AudioNotificationsHandler::AudioNotificationsHandler(
        std::shared_ptr<app::music_player::SongsContract::Presenter> presenter)
        : presenter(presenter)
    {}

    sys::MessagePointer AudioNotificationsHandler::handleAudioStopNotification(
        const AudioStopNotification *notification)
    {
        if (notification == nullptr) {
            return sys::msgNotHandled();
        }

        presenter->handleAudioStopNotifiaction(notification->token);
        return sys::msgHandled();
    }

    sys::MessagePointer AudioNotificationsHandler::handleAudioEofNotification(const AudioStopNotification *notification)
    {
        if (notification == nullptr) {
            return sys::msgNotHandled();
        }

        presenter->handleAudioEofNotification(notification->token);
        return sys::msgHandled();
    }

    sys::MessagePointer AudioNotificationsHandler::handleAudioPausedNotification(
        const AudioPausedNotification *notification)
    {
        if (notification == nullptr) {
            return sys::msgNotHandled();
        }

        presenter->handleAudioPausedNotification(notification->token);
        return sys::msgHandled();
    }

    sys::MessagePointer AudioNotificationsHandler::handleAudioResumedNotification(
        const AudioResumedNotification *notification)
    {
        if (notification == nullptr) {
            return sys::msgNotHandled();
        }

        presenter->handleAudioResumedNotification(notification->token);
        return sys::msgHandled();
    }
} // namespace app::music_player
