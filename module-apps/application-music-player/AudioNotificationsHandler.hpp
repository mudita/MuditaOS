// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <presenters/SongsPresenter.hpp>

class AudioStopNotification;
class AudioPausedNotification;
class AudioResumedNotification;
namespace app::music_player
{
    class AudioNotificationsHandler
    {
      public:
        explicit AudioNotificationsHandler(std::shared_ptr<app::music_player::SongsContract::Presenter> presenter);

        sys::MessagePointer handleAudioStopNotification(const AudioStopNotification *notification);
        sys::MessagePointer handleAudioEofNotification(const AudioStopNotification *notification);
        sys::MessagePointer handleAudioPausedNotification(const AudioPausedNotification *notification);
        sys::MessagePointer handleAudioResumedNotification(const AudioResumedNotification *notification);

      private:
        std::shared_ptr<app::music_player::SongsContract::Presenter> presenter;
    };
} // namespace app::music_player
