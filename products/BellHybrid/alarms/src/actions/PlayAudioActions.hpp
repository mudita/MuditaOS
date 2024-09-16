// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "AbstractAlarmAction.hpp"
#include <audio/AudioMessage.hpp>
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
        PlayAudioAction(sys::Service &service,
                        std::string_view toneSetting,
                        const audio::PlaybackType &playbackType         = audio::PlaybackType::Alarm,
                        const audio::PlaybackMode &playbackMode         = audio::PlaybackMode::Single,
                        std::optional<std::string_view> durationSetting = {});

        auto turnOff() -> bool override;
        auto execute() -> bool override;

      private:
        auto play(const std::filesystem::path &path, std::optional<std::chrono::minutes> duration = {}) -> bool;
        auto spawnTimer(std::chrono::minutes timeout) -> void;
        auto detachTimer() -> void;
        auto getFallbackTonePath() -> std::string;
        auto handleMissingFile() -> std::string;

        sys::Service &service;
        sys::TimerHandle timer;
        const std::string toneSetting;
        const std::optional<std::string> durationSetting;
        const audio::PlaybackType playbackType;
        const audio::PlaybackMode playbackMode;
        settings::Settings settings;
    };

    namespace factory
    {
        std::unique_ptr<PlayAudioAction> createAlarmToneAction(sys::Service &service);
        std::unique_ptr<PlayAudioAction> createPreWakeUpChimeAction(sys::Service &service);
        std::unique_ptr<PlayAudioAction> createSnoozeChimeAction(sys::Service &service);
        std::unique_ptr<PlayAudioAction> createBedtimeChimeAction(sys::Service &service);
    } // namespace factory
} // namespace alarms
