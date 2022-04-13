// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DummyAudioDevice.hpp"

using namespace audio;

class DummyAudioFactory : public AudioDeviceFactory
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
            return std::make_shared<DummyAudioDevice>();
        }
        return nullptr;
    }
};

std::unique_ptr<AudioDeviceFactory> AudioPlatform::GetDeviceFactory()
{
    return std::make_unique<DummyAudioFactory>();
}

auto DummyAudioDevice::Start() -> AudioDevice::RetCode
{
    return AudioDevice::RetCode::Success;
}
auto DummyAudioDevice::Stop() -> AudioDevice::RetCode
{
    return AudioDevice::RetCode::Success;
}
auto DummyAudioDevice::setOutputVolume(float vol) -> AudioDevice::RetCode
{
    return AudioDevice::RetCode::Success;
}
auto DummyAudioDevice::setInputGain(float gain) -> AudioDevice::RetCode
{
    return AudioDevice::RetCode::Success;
}
auto DummyAudioDevice::getTraits() const -> Endpoint::Traits
{
    return Endpoint::Traits();
}
auto DummyAudioDevice::getSupportedFormats() -> std::vector<audio::AudioFormat>
{
    return std::vector<audio::AudioFormat>();
}
auto DummyAudioDevice::getSourceFormat() -> audio::AudioFormat
{
    return audio::AudioFormat();
}

void DummyAudioDevice::onDataSend()
{}
void DummyAudioDevice::onDataReceive()
{}
void DummyAudioDevice::enableInput()
{}
void DummyAudioDevice::enableOutput()
{}
void DummyAudioDevice::disableInput()
{}
void DummyAudioDevice::disableOutput()
{}
