/*
 *  @file RecorderOperation.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 23.07.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

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

        audio::RetCode Start(std::function<int32_t(AudioEvents event)> callback) override final;

        audio::RetCode Stop() override final;

        audio::RetCode Pause() override final;

        audio::RetCode Resume() override final;

        audio::RetCode SendEvent(const Event evt, const EventData *data = nullptr) override final;

        audio::RetCode SwitchProfile(const Profile::Type type) override final;

        audio::RetCode SetOutputVolume(float vol) override final;

        audio::RetCode SetInputGain(float gain) override final;

        Position GetPosition() override final;

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
