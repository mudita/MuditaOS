/*
 *  @file Codec.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 30.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_CODEC_HPP
#define PUREPHONE_CODEC_HPP

#include <memory>
#include <optional>

class CodecParams{
public:

};

enum class CodecRetCode{
    Success,
    InvalidSampleRate,
    InvalidInputPath,
    InvalidOutputPath,
    InvalidArgument
};

class Codec {
public:

    virtual ~Codec(){}

    virtual std::optional<uint32_t > Probe() = 0;

    virtual CodecRetCode Start(const CodecParams& param) = 0;

    virtual CodecRetCode Pause() = 0;

    virtual CodecRetCode Resume() = 0;

    virtual CodecRetCode Stop() = 0;

    virtual CodecRetCode Ioctrl(const CodecParams& param) =0;

};


#endif //PUREPHONE_CODEC_HPP
