// Copyright (c) 2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Audio/AudioDeviceFactory.hpp>
#include <Audio/Profiles/Profile.hpp>

namespace audio
{

    class RT1051DeviceFactory : public AudioDeviceFactory
    {
      public:
        std::shared_ptr<AudioDevice> createCellularAudioDevice() override final;

      protected:
        std::shared_ptr<AudioDevice> getDevice(const Profile &audioProfile) override;
    };

}; // namespace audio
