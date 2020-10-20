// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Operation.hpp"

#include <Audio/encoder/Encoder.hpp>
#include <Audio/AudioCommon.hpp>
#include <Audio/Profiles/Profile.hpp>
#include <bsp/audio/bsp_audio.hpp>

#include <memory>
#include <functional>
#include <string>
#include <vector>

#include <cstdint>

namespace audio
{
    class RouterOperation : public Operation
    {
      public:
        RouterOperation(
            const char *file,
            std::function<std::uint32_t(const std::string &path, const std::uint32_t &defaultValue)> dbCallback);
        ~RouterOperation() = default;

        audio::RetCode Start([[maybe_unused]] audio::AsyncCallback callback, audio::Token token) final;
        audio::RetCode Stop() final;
        audio::RetCode Pause() final;
        audio::RetCode Resume() final;
        audio::RetCode SendEvent(std::shared_ptr<Event> evt) final;
        audio::RetCode SwitchProfile(const Profile::Type type) final;
        audio::RetCode SetOutputVolume(float vol) final;
        audio::RetCode SetInputGain(float gain) final;

        Position GetPosition() final;
        void SetBluetoothStreamData(std::shared_ptr<BluetoothStreamData> data) final;

      private:
        bool Mute(bool enable);

        bool muteEnable = false;

        std::unique_ptr<Encoder> enc;
        std::unique_ptr<bsp::AudioDevice> audioDevice;
        std::unique_ptr<bsp::AudioDevice> audioDeviceCellular;
        std::function<std::int32_t(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer)>
            audioDeviceCallback = nullptr;
        std::function<std::int32_t(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer)>
            audioDeviceCellularCallback = nullptr;
        std::vector<std::int16_t> audioDeviceBuffer;
        std::vector<std::int16_t> audioDeviceCellularBuffer;
        std::vector<std::int16_t> channel1Buffer;
        std::vector<std::int16_t> channel2Buffer;
        std::vector<std::int16_t> mixBuffer;
    };

} // namespace audio
