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

        return presenter->handleAudioStopNotifiaction(notification->token) ? sys::msgNotHandled() : sys::msgHandled();
    }

} // namespace app::music_player
