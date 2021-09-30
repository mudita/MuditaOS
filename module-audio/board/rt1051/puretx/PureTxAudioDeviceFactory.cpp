// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PureTxAudioDeviceFactory.hpp"
#include "PureTxAudioCodec.hpp"
#include "RT1051CellularAudio.hpp"
#include "audio/BluetoothAudioDevice.hpp"

#include <Audio/Profiles/Profile.hpp>

using audio::AudioDevice;
using audio::PureTxAudioCodec;
using audio::PureTxAudioDeviceFactory;
using audio::RT1051CellularAudio;

std::shared_ptr<AudioDevice> PureTxAudioDeviceFactory::getDevice(const audio::Profile &profile)
{
    std::shared_ptr<AudioDevice> device;
    switch (profile.GetAudioDeviceType()) {
    case AudioDevice::Type::Audiocodec: {
        device = std::make_shared<PureTxAudioCodec>(profile.GetAudioConfiguration());
    } break;

    case AudioDevice::Type::BluetoothA2DP: {
        device = std::make_shared<bluetooth::A2DPAudioDevice>();
    } break;

    case AudioDevice::Type::BluetoothHSP: {
        device = std::make_shared<bluetooth::CVSDAudioDevice>(bluetooth::AudioProfile::HSP);
    } break;

    case AudioDevice::Type::BluetoothHFP: {
        device = std::make_shared<bluetooth::CVSDAudioDevice>(bluetooth::AudioProfile::HFP);
    } break;

    case AudioDevice::Type::Cellular: {
        device = std::make_shared<RT1051CellularAudio>();
    } break;

    default:
        break;
    };

    return device;
}

std::shared_ptr<AudioDevice> PureTxAudioDeviceFactory::createCellularAudioDevice()
{
    return std::make_shared<RT1051CellularAudio>();
}
