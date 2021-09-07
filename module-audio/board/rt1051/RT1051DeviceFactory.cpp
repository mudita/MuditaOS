// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051DeviceFactory.hpp"
#include "board/rt1051/RT1051AudioCodec.hpp"
#include "board/rt1051/RT1051CellularAudio.hpp"
#include "audio/BluetoothAudioDevice.hpp"

#include <Audio/Profiles/Profile.hpp>

using audio::AudioDevice;
using audio::RT1051AudioCodec;
using audio::RT1051CellularAudio;
using audio::RT1051DeviceFactory;

std::shared_ptr<AudioDevice> RT1051DeviceFactory::getDevice(const audio::Profile &profile)
{
    std::shared_ptr<AudioDevice> device;
    switch (profile.GetAudioDeviceType()) {
    case AudioDevice::Type::Audiocodec: {
        device = std::make_shared<RT1051AudioCodec>(profile.GetAudioConfiguration());
    } break;

    case AudioDevice::Type::BluetoothA2DP: {
        device = std::make_shared<bluetooth::A2DPAudioDevice>();
    } break;

    case AudioDevice::Type::BluetoothHSP: {
        device = std::make_shared<bluetooth::HSPAudioDevice>();
    } break;

    case AudioDevice::Type::Cellular: {
        device = std::make_shared<RT1051CellularAudio>();
    } break;

    default:
        break;
    };

    return device;
}

std::shared_ptr<AudioDevice> RT1051DeviceFactory::createCellularAudioDevice()
{
    return std::make_shared<RT1051CellularAudio>();
}
