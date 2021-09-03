// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PlayToneAction.hpp"
#include <module-vfs/include/user/purefs/filesystem_paths.hpp>
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
    alarms::PlayToneAction::PlayToneAction(sys::Service &service) : service{service}
    {
        settings.init(service::ServiceProxy{service.weak_from_this()});
    }
    bool alarms::PlayToneAction::execute()
    {
        const auto valueStr = settings.getValue(bell::settings::Ringing::duration, settings::SettingsScope::Global);
        const auto ringingDuration = std::chrono::seconds{utils::getNumericValue<uint32_t>(valueStr)};
        const auto tone            = settings.getValue(bell::settings::Ringing::tone, settings::SettingsScope::Global);
        const auto tonePath        = std::filesystem::path{purefs::dir::getUserDiskPath().string() + musicDir + tone};

        spawnTimer(ringingDuration);

        return AudioServiceAPI::PlaybackStart(&service, audio::PlaybackType::Alarm, tonePath);
    }

    bool alarms::PlayToneAction::turnOff()
    {
        return true;
    }

    void PlayToneAction::detachTimer()
    {
        if (timer.isValid()) {
            timer.stop();
            timer.reset();
        }
    }
    void PlayToneAction::spawnTimer(std::chrono::seconds timeout)
    {
        if (not timer.isValid()) {
            auto callback = [this](sys::Timer &) { AudioServiceAPI::Stop(&service, {audio::PlaybackType::Alarm}); };
            timer         = sys::TimerFactory::createSingleShotTimer(&service, timerName, timeout, callback);
        }
        timer.stop();
        timer.start();
    }

} // namespace alarms
