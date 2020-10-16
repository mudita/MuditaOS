// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Operation.hpp"
#include <Audio/encoder/Encoder.hpp>
#include <bsp/audio/bsp_audio.hpp>

namespace audio
{

    class RecorderOperation : public Operation
    {
      public:
        RecorderOperation(const char *file,
                          std::function<uint32_t(const std::string &path, const uint32_t &defaultValue)> dbCallback);

        audio::RetCode Start(audio::AsyncCallback callback, audio::Token token) final;
        audio::RetCode Stop() final;
        audio::RetCode Pause() final;
        audio::RetCode Resume() final;
        audio::RetCode SendEvent(std::shared_ptr<Event> evt) final;
        audio::RetCode SwitchProfile(const Profile::Type type) final;
        audio::RetCode SetOutputVolume(float vol) final;
        audio::RetCode SetInputGain(float gain) final;

        Position GetPosition() final;
        void SetBluetoothStreamData(BluetoothStreamData data) final;

        uint32_t GetSize()
        {
            return enc->GetFileSize();
        }

      private:
        std::unique_ptr<Encoder> enc;
        std::unique_ptr<bsp::AudioDevice> audioDevice;
    };

} // namespace audio
