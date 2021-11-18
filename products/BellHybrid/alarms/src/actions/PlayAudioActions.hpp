// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractAlarmAction.hpp"
#include <audio/AudioMessage.hpp>
#include <common/SoundsRepository.hpp>
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
        bool execute() override;
        explicit PlayAudioAction(sys::Service &service,
                                 const std::filesystem::path &tonesDirPath,
                                 std::string_view toneSetting,
                                 std::string_view durationSetting,
                                 audio::PlaybackType = audio::PlaybackType::Alarm);

      private:
        static constexpr auto InfiniteDuration = std::chrono::minutes::max();
        static constexpr auto NoDuration       = std::chrono::minutes::zero();

        bool play(const std::filesystem::path &path, std::chrono::minutes duration = InfiniteDuration);
        void spawnTimer(std::chrono::minutes timeout);
        void detachTimer();

        sys::Service &service;
        sys::TimerHandle timer;
        SoundsRepository soundsRepository;
        const std::string toneSetting;
        const std::string durationSetting;
        const audio::PlaybackType playbackType;
        settings::Settings settings;
    };

    namespace factory
    {
        static constexpr auto NoPlaybackTimeout = "0";
        std::unique_ptr<PlayAudioAction> createAlarmToneAction(sys::Service &service);
        std::unique_ptr<PlayAudioAction> createPreWakeUpChimeAction(sys::Service &service);
        std::unique_ptr<PlayAudioAction> createSnoozeChimeAction(sys::Service &service);
        std::unique_ptr<PlayAudioAction> createBedtimeChimeAction(sys::Service &service);
    } // namespace factory
} // namespace alarms
