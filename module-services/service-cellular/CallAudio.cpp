// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallAudio.hpp"
#include <service-audio/AudioServiceAPI.hpp>

CallRingAudio::CallRingAudio(sys::Service &s) : owner(s)
{}

void CallRingAudio::play()
{
    const auto filePath = AudioServiceAPI::GetSound(&owner, audio::PlaybackType::CallRingtone);
    AudioServiceAPI::PlaybackStart(&owner, audio::PlaybackType::CallRingtone, filePath);
}

void CallRingAudio::stop()
{
    /// NOTE: we should fix it to stop actual played ringtone...
    /// but we would require async response from PlayBack start or API in audio to be able to
    /// just stop Ringtone by path
    AudioServiceAPI::StopAll(&owner);
}
