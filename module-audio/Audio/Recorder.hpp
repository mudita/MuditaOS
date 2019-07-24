/*
 *  @file Recorder.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 23.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_RECORDER_HPP
#define PUREPHONE_RECORDER_HPP


#include "Operation.hpp"
#include <Audio/encoder/Encoder.hpp>

class Encoder;

class Recorder : public Operation{
public:


    Recorder(const char *file, const Profile* profile,const Encoder::Format& frmt);
    virtual ~Recorder();

    int32_t Start(std::function<int32_t (uint32_t)> callback) override final;

    int32_t Stop() override final;

    int32_t Pause() override final;

    int32_t Resume() override final;

    int32_t SwitchProfile(const Profile* prof)  override final;

    Position GetPosition() override final;

    uint32_t GetSize(){return enc->GetFileSize();}

private:
    std::unique_ptr<Encoder> enc;
    const Encoder::Format format;
};


#endif //PUREPHONE_RECORDER_HPP
