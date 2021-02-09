// Copyright (c) 2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Audio/AudioDeviceFactory.hpp>

namespace audio
{

    class RT1051DeviceFactory : public AudioDeviceFactory
    {
      protected:
        std::shared_ptr<AudioDevice> getDeviceFromType(AudioDevice::Type deviceType) override;
    };

}; // namespace audio
