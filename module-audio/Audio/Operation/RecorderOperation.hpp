// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef PUREPHONE_RECORDEROPERATION_HPP
#define PUREPHONE_RECORDEROPERATION_HPP

#include "Operation.hpp"
#include <Audio/encoder/Encoder.hpp>
#include "bsp/audio/bsp_audio.hpp"

namespace audio
{

    class RecorderOperation : public Operation
    {
      public:
        RecorderOperation(const char *file,
                          std::function<uint32_t(const std::string &path, const uint32_t &defaultValue)> dbCallback);

        audio::RetCode Start(audio::AsyncCallback callback, audio::Token token) override final;

        audio::RetCode Stop() override final;

        audio::RetCode Pause() override final;

        audio::RetCode Resume() override final;

        audio::RetCode SendEvent(std::shared_ptr<Event> evt) override final;

        audio::RetCode SwitchProfile(const Profile::Type type) override final;

        audio::RetCode SetOutputVolume(float vol) override final;

        audio::RetCode SetInputGain(float gain) override final;

        Position GetPosition() override final;

        void SetBluetoothStreamData(BluetoothStreamData data) override final;

        uint32_t GetSize()
        {
            return enc->GetFileSize();
        }

      private:
        std::unique_ptr<Encoder> enc;
        std::unique_ptr<bsp::AudioDevice> audioDevice;
    };

} // namespace audio

#endif // PUREPHONE_RECORDEROPERATION_HPP
