// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PlayAudioActions.hpp"
#include <purefs/filesystem_paths.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <service-time/ServiceTime.hpp>
#include <db/SystemSettings.hpp>
#include <Timers/TimerFactory.hpp>

namespace
{
    constexpr auto timerName = "playDurationTimer";
    constexpr auto musicDir  = "/music/";
} // namespace

namespace alarms
{
    PlayAudioAction::PlayAudioAction(sys::Service &service) : service{service}
    {}

    bool PlayAudioAction::play(const std::filesystem::path &path, std::chrono::seconds duration)
    {
        if (duration != InfiniteDuration) {
            spawnTimer(duration);
        }
        return AudioServiceAPI::PlaybackStart(&service, audio::PlaybackType::Alarm, path);
    }

    bool PlayAudioAction::turnOff()
    {
        auto stopPlaybackVec = std::vector<audio::PlaybackType>({audio::PlaybackType::Alarm});
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
            auto callback = [this](sys::Timer &) { AudioServiceAPI::Stop(&service, {audio::PlaybackType::Alarm}); };
            timer         = sys::TimerFactory::createSingleShotTimer(&service, timerName, timeout, callback);
        }
        timer.stop();
        timer.start();
    }

    PlayToneAction::PlayToneAction(sys::Service &service) : PlayAudioAction{service}
    {
        settings.init(service::ServiceProxy{service.weak_from_this()});
    }

    bool PlayToneAction::execute()
    {
        const auto tone     = settings.getValue(bell::settings::Ringing::tone, settings::SettingsScope::Global);
        const auto tonePath = std::filesystem::path{purefs::dir::getUserDiskPath().string() + musicDir + tone};
        const auto valueStr = settings.getValue(bell::settings::Ringing::duration, settings::SettingsScope::Global);
        const auto ringingDuration = std::chrono::seconds{utils::getNumericValue<uint32_t>(valueStr)};
        return play(tonePath, ringingDuration);
    }

    PlayChimeAction::PlayChimeAction(sys::Service &service) : PlayAudioAction{service}
    {
        settings.init(service::ServiceProxy{service.weak_from_this()});
    }

    bool PlayChimeAction::execute()
    {
        const auto tone     = settings.getValue(bell::settings::Ringing::tone, settings::SettingsScope::Global);
        const auto tonePath = std::filesystem::path{purefs::dir::getUserDiskPath().string() + musicDir + tone};
        return play(tonePath);
    }
} // namespace alarms
