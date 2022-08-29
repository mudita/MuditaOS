// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Operation.hpp"
#include <Audio/encoder/Encoder.hpp>

namespace audio
{

    class RecorderOperation : public Operation
    {
      public:
        RecorderOperation(const char *file, AudioServiceMessage::Callback callback);

        audio::RetCode Start(audio::Token token) final;
        audio::RetCode Stop() final;
        audio::RetCode Pause() final;
        audio::RetCode Resume() final;
        audio::RetCode SendEvent(std::shared_ptr<Event> evt) final;
        audio::RetCode SwitchProfile(const Profile::Type type) final;
        audio::RetCode SetOutputVolume(float vol) final;
        audio::RetCode SetInputGain(float gain) final;

        Position GetPosition() final;

        uint32_t GetSize()
        {
            return enc->GetFileSize();
        }

      private:
        std::unique_ptr<Encoder> enc;
    };

} // namespace audio
