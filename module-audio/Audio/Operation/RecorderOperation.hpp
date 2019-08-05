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

namespace audio {


    class RecorderOperation : public Operation {
    public:


        RecorderOperation(const char *file);

        int32_t Start(std::function<int32_t(AudioEvents event)> callback) override final;

        int32_t Stop() override final;

        int32_t Pause() override final;

        int32_t Resume() override final;

        int32_t SendEvent(const Event evt, const EventData *data = nullptr) override final;

        int32_t SwitchProfile(const Profile::Type type) override final;

        int32_t SetOutputVolume(float vol) override final;

        int32_t SetInputGain(float gain) override final;

        Position GetPosition() override final;

        uint32_t GetSize() { return enc->GetFileSize(); }

    private:
        std::unique_ptr<Encoder> enc;
        std::unique_ptr<bsp::AudioDevice> audioDevice;
    };

}


#endif //PUREPHONE_RECORDEROPERATION_HPP
