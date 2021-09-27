// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PlayAudioActions.hpp"
#include "log/log.hpp"
#include <chrono>
#include <purefs/filesystem_paths.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <service-time/ServiceTime.hpp>
#include <Timers/TimerFactory.hpp>

namespace
{
    constexpr auto timerName = "playDurationTimer";
    constexpr std::chrono::seconds defaultTimerTime{30};
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

    bool PlayAudioAction::turnOff(const AlarmEventRecord & /*record*/)
    {
        return AudioServiceAPI::Stop(&service, {audio::PlaybackType::Alarm});
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

    bool PlayToneAction::execute(const AlarmEventRecord &record)
    {
        const auto tonePath = std::filesystem::path{record.musicTone};
        LOG_DEBUG("play some music: %s", record.musicTone.c_str());
        return play(tonePath, defaultTimerTime);
    }
} // namespace alarms
