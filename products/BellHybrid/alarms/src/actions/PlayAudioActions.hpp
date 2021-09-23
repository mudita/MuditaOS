// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractAlarmAction.hpp"
#include <service-audio/AudioServiceAPI.hpp>
#include <module-audio/Audio/AudioCommon.hpp>
#include <service-db/Settings.hpp>
#include <Timers/TimerHandle.hpp>
#include <Service/Service.hpp>

#include <chrono>
#include <filesystem>

namespace alarms
{
    class PlayAudioAction : public AbstractAlarmAction
    {
      public:
        bool turnOff() override;

      protected:
        explicit PlayAudioAction(sys::Service &service, audio::PlaybackType = audio::PlaybackType::Alarm);

        static constexpr auto InfiniteDuration = std::chrono::seconds::max();
        bool play(const std::filesystem::path &path, std::chrono::seconds duration = InfiniteDuration);

      private:
        void spawnTimer(std::chrono::seconds timeout);
        void detachTimer();

        sys::Service &service;
        sys::TimerHandle timer;
        audio::PlaybackType playbackType;
    };

    class PlayToneAction : public PlayAudioAction
    {
      public:
        explicit PlayToneAction(sys::Service &service);
        bool execute() override;

      private:
        settings::Settings settings;
    };

    class PlayChimeAction : public PlayAudioAction
    {
      public:
        explicit PlayChimeAction(sys::Service &service,
                                 const std::filesystem::path &tonesDirPath,
                                 std::string_view toneSetting);
        bool execute() override;

      private:
        settings::Settings settings;
        std::filesystem::path tonesDirPath;
        std::string_view toneSetting;
    };

    std::unique_ptr<PlayChimeAction> createPreWakeUpChimeAction(sys::Service &service);
    std::unique_ptr<PlayChimeAction> createSnoozeChimeAction(sys::Service &service);
} // namespace alarms
