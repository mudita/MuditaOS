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
#include <gsl/util>

struct Audio::CallMeta
{
    sys::Async<AudioStartPlaybackRequest, AudioStartPlaybackResponse> async;
};

Audio::Audio(sys::Service *s) : owner(s), meta(new Audio::CallMeta)
{}

Audio::~Audio()
{
    delete meta;
}

void Audio::play()
{
    if (not started) {
        started         = true;
        const auto file = AudioServiceAPI::GetSound(owner, audio::PlaybackType::CallRingtone);
        meta->async     = owner->async_call<AudioStartPlaybackRequest, AudioStartPlaybackResponse>(
            service::name::audio, file, audio::PlaybackType::CallRingtone);
    }
}

void Audio::stop()
{
    auto _ = gsl::finally([this] { AudioServiceAPI::StopAll(owner); });

    if (not started) {
        return;
    }
    started = false;
    owner->sync(meta->async);
}

void Audio::muteCall()
{
    AudioServiceAPI::SendEvent(owner, audio::EventType::CallMute);
}

void Audio::unmuteCall()
{
    AudioServiceAPI::SendEvent(owner, audio::EventType::CallUnmute);
}

void Audio::setLoudspeakerOn()
{
    AudioServiceAPI::SendEvent(owner, audio::EventType::CallLoudspeakerOn);
}

void Audio::setLoudspeakerOff()
{
    AudioServiceAPI::SendEvent(owner, audio::EventType::CallLoudspeakerOff);
}

void Audio::routingStart()
{
    AudioServiceAPI::RoutingStart(owner);
}
