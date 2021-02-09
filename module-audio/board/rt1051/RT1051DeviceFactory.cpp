// Copyright (c) 2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051DeviceFactory.hpp"
#include "board/rt1051/RT1051AudioCodec.hpp"
#include "board/rt1051/RT1051CellularAudio.hpp"

using audio::AudioDevice;
using audio::RT1051AudioCodec;
using audio::RT1051CellularAudio;
using audio::RT1051DeviceFactory;

std::shared_ptr<AudioDevice> RT1051DeviceFactory::getDeviceFromType(AudioDevice::Type deviceType)
{
    std::shared_ptr<AudioDevice> device;
    switch (deviceType) {
    case AudioDevice::Type::Audiocodec: {
        device = std::make_unique<RT1051AudioCodec>();
    } break;

    case AudioDevice::Type::Bluetooth: {
        LOG_FATAL("Bluetooth audio is not yet supported");
        device = nullptr;
    } break;

    case AudioDevice::Type::Cellular: {
        device = std::make_unique<RT1051CellularAudio>();
    } break;

    default:
        break;
    };

    return device;
}
