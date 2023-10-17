// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Audio/AudioPlatform.hpp>

#if BOARD_puretx == 1
#include <PureTxAudioDeviceFactory.hpp>
using audio::PureTxAudioDeviceFactory;
#elif BOARD_bellpx == 1
#include <BellPxAudioDeviceFactory.hpp>
using audio::BellPxAudioDeviceFactory;
#else
#error "Unsupported board type"
#endif

#include <utility>

using audio::AudioDeviceFactory;
using audio::AudioPlatform;

std::unique_ptr<AudioDeviceFactory> AudioPlatform::GetDeviceFactory()
{
#if BOARD_puretx == 1
    return std::make_unique<PureTxAudioDeviceFactory>();
#elif BOARD_bellpx == 1
    return std::make_unique<BellPxAudioDeviceFactory>();
#else
#error "Unsupported board type"
#endif
}
