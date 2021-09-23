// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmSoundPaths.hpp"
#include "PlayAudioActions.hpp"

#include <service-time/ServiceTime.hpp>
#include <db/SystemSettings.hpp>
#include <purefs/filesystem_paths.hpp>
#include <Timers/TimerFactory.hpp>

namespace
{
    constexpr auto timerName = "playDurationTimer";
} // namespace

namespace alarms
{
    PlayAudioAction::PlayAudioAction(sys::Service &service, audio::PlaybackType playbackType)
        : service{service}, playbackType{playbackType}
    {}

    bool PlayAudioAction::play(const std::filesystem::path &path, std::chrono::seconds duration)
    {
        if (duration != InfiniteDuration) {
            spawnTimer(duration);
        }
        return AudioServiceAPI::PlaybackStart(&service, playbackType, path);
    }

    bool PlayAudioAction::turnOff()
    {
        auto stopPlaybackVec = std::vector<audio::PlaybackType>({playbackType});
        return AudioServiceAPI::Stop(&service, stopPlaybackVec);
    }

    void PlayAudioAction::detachTimer()
    {
        if (timer.isValid()) {
            timer.stop();
            timer.reset();
        }
    }
    void PlayAudioAction::spawnTimer(std::chrono::seconds timeout)
    {
        if (not timer.isValid()) {
            auto callback = [this](sys::Timer &) { turnOff(); };
            timer         = sys::TimerFactory::createSingleShotTimer(&service, timerName, timeout, callback);
        }
        timer.stop();
        timer.start();
    }

    PlayToneAction::PlayToneAction(sys::Service &service) : PlayAudioAction{service, audio::PlaybackType::Alarm}
    {
        settings.init(service::ServiceProxy{service.weak_from_this()});
    }

    bool PlayToneAction::execute()
    {
        const auto tone     = settings.getValue(bell::settings::Ringing::tone, settings::SettingsScope::Global);
        const auto tonePath = paths::getMusicDir() / tone;
        const auto valueStr = settings.getValue(bell::settings::Ringing::duration, settings::SettingsScope::Global);
        const auto ringingDuration = std::chrono::seconds{utils::getNumericValue<uint32_t>(valueStr)};
        return play(tonePath, ringingDuration);
    }

    std::unique_ptr<PlayChimeAction> createPreWakeUpChimeAction(sys::Service &service)
    {
        return std::make_unique<PlayChimeAction>(
            service, paths::getPreWakeUpChimesDir(), bell::settings::PrewakeUp::tone);
    }

    std::unique_ptr<PlayChimeAction> createSnoozeChimeAction(sys::Service &service)
    {
        return std::make_unique<PlayChimeAction>(service, paths::getSnoozeChimesDir(), bell::settings::Snooze::tone);
    }

    PlayChimeAction::PlayChimeAction(sys::Service &service,
                                     const std::filesystem::path &tonesDirPath,
                                     std::string_view toneSetting)
        : PlayAudioAction{service, audio::PlaybackType::Multimedia}, tonesDirPath{tonesDirPath}, toneSetting{
                                                                                                     toneSetting}
    {
        settings.init(service::ServiceProxy{service.weak_from_this()});
    }

    bool PlayChimeAction::execute()
    {
        const auto tone     = settings.getValue(toneSetting.data(), settings::SettingsScope::Global);
        const auto tonePath = tonesDirPath / tone;
        return play(tonePath);
    }
} // namespace alarms
