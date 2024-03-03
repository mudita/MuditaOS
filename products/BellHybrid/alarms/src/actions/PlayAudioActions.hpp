// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AbstractAlarmAction.hpp"
#include <audio/AudioMessage.hpp>
#include <service-db/Settings.hpp>
#include <Timers/TimerHandle.hpp>

#include <chrono>
#include <filesystem>

namespace sys
{
    class Service;
}

namespace alarms
{
    class PlayAudioAction : public AbstractAlarmAction
    {
      public:
        PlayAudioAction(sys::Service &service,
                        std::string_view toneSetting,
                        audio::PlaybackType                             = audio::PlaybackType::Alarm,
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
