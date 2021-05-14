// Copyright (c) 2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Audio/AudioPlatform.hpp>
#include <Audio/Profiles/Profile.hpp>

#include <memory>
#include <utility>

using audio::AudioDevice;
using audio::AudioDeviceFactory;
using audio::AudioPlatform;

class DummyAudioFactory : public AudioDeviceFactory
{
  protected:
    std::shared_ptr<AudioDevice> getDevice([[maybe_unused]] audio::Profile profile)
    {
        return nullptr;
    }
};

std::unique_ptr<AudioDeviceFactory> AudioPlatform::GetDeviceFactory()
{
    return std::make_unique<DummyAudioFactory>();
}
