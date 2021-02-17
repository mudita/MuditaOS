// Copyright (c) 2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Audio/AudioPlatform.hpp>

#include "RT1051DeviceFactory.hpp"

#include <utility>

using audio::AudioDeviceFactory;
using audio::AudioPlatform;

std::unique_ptr<AudioDeviceFactory> AudioPlatform::GetDeviceFactory()
{
    return std::make_unique<RT1051DeviceFactory>();
}
