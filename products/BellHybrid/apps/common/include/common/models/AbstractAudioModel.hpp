// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Audio/AudioCommon.hpp>

#include <string>
#include <functional>

namespace app
{
    using AlarmModelReadyHandler = std::function<void()>;

    class AbstractAudioModel
    {
      public:
        /// 0-10 range
        using Volume = std::uint8_t;

        using OnPlayCallback   = std::function<void(audio::RetCode retCode, audio::Token token)>;
        using OnStopCallback   = OnPlayCallback;
        using OnPauseCallback  = OnPlayCallback;
        using OnResumeCallback = OnPlayCallback;

        enum class PlaybackType
        {
            Chime,
            Alarm,
            PreWakeup,
            Bedtime
        };

        virtual ~AbstractAudioModel() noexcept                                                            = default;
        virtual void setVolume(Volume volume, PlaybackType playbackType)                                  = 0;
        virtual bool play(const std::string &filePath, const OnPlayCallback &callback, PlaybackType type) = 0;
        virtual bool pause(const audio::Token &token, const OnPauseCallback &callback)                    = 0;
        virtual bool resume(const audio::Token &token, const OnResumeCallback &callback)                  = 0;
        virtual bool stop(const audio::Token &token, const OnStopCallback &callback)                      = 0;
    };

} // namespace app
