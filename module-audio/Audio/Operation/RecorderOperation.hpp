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

class Encoder;

namespace bsp{
    class AudioDevice;
}

class RecorderOperation : public Operation{
public:


    RecorderOperation(const char *file,const Encoder::Format& frmt);

    int32_t Start(std::function<int32_t (uint32_t)> callback) override final;

    int32_t Stop() override final;

    int32_t Pause() override final;

    int32_t Resume() override final;

    int32_t SwitchProfile(const Profile::Type type)  override final;

    Position GetPosition() override final;

    uint32_t GetSize(){return enc->GetFileSize();}

private:
    std::unique_ptr<Encoder> enc;
    std::unique_ptr<bsp::AudioDevice> audioDevice;
    const Encoder::Format format;
};


#endif //PUREPHONE_RECORDEROPERATION_HPP
