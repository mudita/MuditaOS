// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Operation.hpp"

#include <Audio/encoder/Encoder.hpp>
#include <Audio/AudioCommon.hpp>
#include <Audio/Profiles/Profile.hpp>
#include <Audio/Endpoint.hpp>
#include <bsp/audio/bsp_audio.hpp>
#include <mutex.hpp>

#include <memory>
#include <functional>
#include <string>
#include <vector>

#include <cstdint>

namespace audio
{
    class RouterOperation : public Operation
    {
        using AudioCallback =
            std::function<std::int32_t(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer)>;
        static const std::size_t INPUT_BUFFER_START_SIZE = 1024;

      public:
        RouterOperation(const char *file, AudioServiceMessage::Callback callback);
        ~RouterOperation();

        audio::RetCode Start(audio::Token token) final;
        audio::RetCode Stop() final;
        audio::RetCode Pause() final;
        audio::RetCode Resume() final;
        audio::RetCode SendEvent(std::shared_ptr<Event> evt) final;
        audio::RetCode SwitchProfile(const Profile::Type type) final;
        audio::RetCode SetOutputVolume(float vol) final;
        audio::RetCode SetInputGain(float gain) final;

        Position GetPosition() final;

      private:
        bool Mute(bool enable);

        bool muteEnable = false;
        std::unique_ptr<Encoder> enc;
        std::unique_ptr<bsp::AudioDevice> audioDeviceCellular;
        std::unique_ptr<StreamConnection> outputConnection;
        std::unique_ptr<StreamConnection> inputConnection;
    };

} // namespace audio
