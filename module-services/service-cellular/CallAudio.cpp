// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallAudio.hpp"
#include "Service/Message.hpp"
#include "products/BellHybrid/services/audio/include/audio/AudioMessage.hpp"
#include <service-audio/AudioServiceAPI.hpp>
#include <Timers/TimerFactory.hpp>

struct CallRingAudio::CallMeta
{
    audio::Token token;
    sys::TimerHandle retryTimer;
};

CallRingAudio::CallRingAudio(sys::Service &s) : owner(s), meta(new CallRingAudio::CallMeta)
{}

void CallRingAudio::play()
{
    const auto filePath = AudioServiceAPI::GetSound(&owner, audio::PlaybackType::CallRingtone);
    AudioServiceAPI::PlaybackStart(&owner, audio::PlaybackType::CallRingtone, filePath);
    owner.connect(typeid(service::AudioStartPlaybackResponse), [this](sys::Message *msg) -> sys::MessagePointer {
        auto *message = dynamic_cast<service::AudioStartPlaybackResponse *>(msg);
        assert(meta);
        meta->token = message->token;
        owner.disconnect(typeid(service::AudioStartPlaybackResponse));
        return sys::msgHandled();
    });
}

void CallRingAudio::stop()
{
    assert(meta && "we have stop without start - this should never happen");
    // there is still a chance we didn't get the token - in this case, maybe timer? :D
    if (!meta->token.IsValid() && !meta->retryTimer.isActive()) {
        meta->retryTimer = sys::TimerFactory::createPeriodicTimer(
            &owner, "retry stop music", std::chrono::seconds{1}, [this](sys::Timer &) {
                meta->token.IsValid();
                AudioServiceAPI::Stop(&owner, meta->token);
                meta->retryTimer.stop();
            });
        meta->retryTimer.start();
    }
    AudioServiceAPI::Stop(&owner, meta->token);
}
