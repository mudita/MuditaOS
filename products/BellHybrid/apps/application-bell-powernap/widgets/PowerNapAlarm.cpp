// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerNapAlarm.hpp"

#include <purefs/filesystem_paths.hpp>
#include <service-audio/AudioServiceAPI.hpp>

namespace
{
    constexpr auto alarmAudioPath = "assets/audio/alarm/alarm_mudita_bell.mp3";
}

namespace app::powernap
{
    PowerNapAlarmImpl::PowerNapAlarmImpl(app::ApplicationCommon *app) : app{app}
    {}

    void PowerNapAlarmImpl::start()
    {
        AudioServiceAPI::PlaybackStart(
            app, audio::PlaybackType::Alarm, purefs::dir::getCurrentOSPath() / alarmAudioPath);
    }

    void PowerNapAlarmImpl::stop()
    {
        if (token.IsValid()) {
            AudioServiceAPI::Stop(app, token);
            token = audio::Token::MakeBadToken();
        }
    }

    void PowerNapAlarmImpl::registerAudioStream(audio::Token _token)
    {
        token = _token;
    }
} // namespace app::powernap
