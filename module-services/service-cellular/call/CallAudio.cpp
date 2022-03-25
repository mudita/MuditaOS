// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallAudio.hpp"
#include "Service/Message.hpp"
#include "log/log.hpp"
#include <limits>
#include <service-audio/AudioMessage.hpp>
#include "service-audio/AudioServiceName.hpp"
#include <service-audio/AudioServiceAPI.hpp>
#include <Timers/TimerFactory.hpp>

struct CallRingAudio::CallMeta
{
    sys::Async<AudioStartPlaybackRequest, AudioStartPlaybackResponse> async;
};

CallRingAudio::CallRingAudio(sys::Service &s) : owner(s), meta(new CallRingAudio::CallMeta)
{}

CallRingAudio::~CallRingAudio()
{
    delete meta;
}

void CallRingAudio::play()
{
    started         = true;
    const auto file = AudioServiceAPI::GetSound(&owner, audio::PlaybackType::CallRingtone);
    meta->async     = owner.async_call<AudioStartPlaybackRequest, AudioStartPlaybackResponse>(
        service::name::audio, file, audio::PlaybackType::CallRingtone);
}

void CallRingAudio::stop()
{
    if (not started) {
        return;
    }
    owner.sync(meta->async);
    AudioServiceAPI::StopAll(&owner);
}

void CallRingAudio::muteCall()
{
    AudioServiceAPI::SendEvent(&owner, audio::EventType::CallMute);
}

void CallRingAudio::unmuteCall()
{
    AudioServiceAPI::SendEvent(&owner, audio::EventType::CallUnmute);
}

void CallRingAudio::setLaudspeakerOn()
{
    AudioServiceAPI::SendEvent(&owner, audio::EventType::CallLoudspeakerOn);
}

void CallRingAudio::setLaudspeakerOff()
{
    AudioServiceAPI::SendEvent(&owner, audio::EventType::CallLoudspeakerOff);
}
