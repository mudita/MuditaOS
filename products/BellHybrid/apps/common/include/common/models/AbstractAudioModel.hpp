// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <module-audio/Audio/AudioCommon.hpp>
#include <string>
#include <functional>
#include <optional>

namespace app
{
    class AbstractAudioModel
    {
      public:
        enum class PlaybackMode
        {
            Single,
            Loop
        };

        enum class PlaybackType
        {
            Multimedia,
            Snooze,
            Alarm,
            PreWakeup,
            Bedtime,
            Meditation,
            FocusTimer
        };

        enum class PlaybackFinishStatus
        {
            Normal,
            Error
        };

        /// 0-15 range
        static constexpr auto minVolume  = 1;
        static constexpr auto maxVolume  = 15;
        using Volume                     = std::uint32_t;
        using OnStateChangeCallback      = std::function<void(const audio::RetCode code)>;
        using OnGetValueCallback         = std::function<void(const audio::RetCode, Volume)>;
        using OnPlaybackFinishedCallback = std::function<void(PlaybackFinishStatus)>;

        virtual ~AbstractAudioModel() noexcept                             = default;
        virtual void setVolume(Volume volume,
                               PlaybackType playbackType,
                               audio::VolumeUpdateType updateType = audio::VolumeUpdateType::UpdateDB,
                               OnStateChangeCallback &&callback   = {})      = 0;
        virtual std::optional<Volume> getVolume(PlaybackType playbackType) = 0;

        virtual void getVolume(PlaybackType playbackType, OnGetValueCallback &&callback) = 0;
        virtual void play(const std::string &filePath,
                          const PlaybackType &type,
                          const PlaybackMode &mode,
                          OnStateChangeCallback &&callback,
                          std::optional<audio::FadeParams> fadeParams = std::nullopt)    = 0;
        virtual void stopAny(OnStateChangeCallback &&callback)                           = 0;
        virtual void stopPlayedByThis(OnStateChangeCallback &&callback)                  = 0;
        virtual void pause(OnStateChangeCallback &&callback)                             = 0;
        virtual void resume(OnStateChangeCallback &&callback)                            = 0;
        virtual void setPlaybackFinishedCb(OnPlaybackFinishedCallback &&callback)        = 0;
        virtual bool hasPlaybackFinished()                                               = 0;
    };

} // namespace app
