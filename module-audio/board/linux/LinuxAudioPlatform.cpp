// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Audio/AudioPlatform.hpp>
#include <Audio/Profiles/Profile.hpp>
#include <board/linux/LinuxAudioDevice.hpp>

#include <memory>
#include <utility>

using audio::AudioDevice;
using audio::AudioDeviceFactory;
using audio::AudioPlatform;

class LinuxAudioFactory : public AudioDeviceFactory
{
  public:
    std::shared_ptr<AudioDevice> createCellularAudioDevice() override
    {
        return nullptr;
    }

  protected:
    std::shared_ptr<AudioDevice> getDevice([[maybe_unused]] const audio::Profile &profile) override
    {
        if (profile.GetAudioDeviceType() == AudioDevice::Type::Audiocodec) {
            return std::make_shared<audio::LinuxAudioDevice>(profile.GetAudioConfiguration().outputVolume);
        }
        return nullptr;
    }
};

std::unique_ptr<AudioDeviceFactory> AudioPlatform::GetDeviceFactory()
{
    return std::make_unique<LinuxAudioFactory>();
}
