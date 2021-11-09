// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
        /// 0-10 range
        static constexpr auto minVolume = 1;
        static constexpr auto maxVolume = 10;
        using Volume                    = std::uint32_t;
        using OnStateChangeCallback     = std::function<void(const audio::RetCode code)>;
        using OnGetValueCallback        = std::function<void(const audio::RetCode, Volume)>;

        enum class PlaybackType
        {
            Multimedia,
            Snooze,
            Alarm,
            PreWakeup,
            Bedtime
        };

        virtual ~AbstractAudioModel() noexcept                                                              = default;
        virtual void setVolume(Volume volume, PlaybackType playbackType, OnStateChangeCallback &&callback)  = 0;
        virtual std::optional<Volume> getVolume(PlaybackType playbackType)                                  = 0;
        virtual void getVolume(PlaybackType playbackType, OnGetValueCallback &&callback)                    = 0;
        virtual void play(const std::string &filePath, PlaybackType type, OnStateChangeCallback &&callback) = 0;
        virtual void stop(OnStateChangeCallback &&callback)                                                 = 0;
        virtual void pause(OnStateChangeCallback &&callback)                                                = 0;
        virtual void resume(OnStateChangeCallback &&callback)                                               = 0;
    };

} // namespace app
