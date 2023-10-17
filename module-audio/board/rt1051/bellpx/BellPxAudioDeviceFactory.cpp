// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellPxAudioDeviceFactory.hpp"
#include "BellPxAudioCodec.hpp"

#include <Audio/Profiles/Profile.hpp>

#include <log/log.hpp>

#include <stdexcept>

using audio::AudioDevice;
using audio::BellPxAudioDeviceFactory;

std::shared_ptr<AudioDevice> BellPxAudioDeviceFactory::getDevice(const audio::Profile &profile)
{
    std::shared_ptr<AudioDevice> device;

    switch (profile.GetAudioDeviceType()) {
    case AudioDevice::Type::Audiocodec: {
        device = std::make_shared<BellPxAudioCodec>(profile.GetAudioConfiguration());
    } break;

    default:
        break;
    };

    return device;
}

std::shared_ptr<AudioDevice> BellPxAudioDeviceFactory::createCellularAudioDevice()
{
    throw std::runtime_error("Cellular audio device not valid for Bell platform.");
}
