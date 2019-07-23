/*
 *  @file Encoder.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 23.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_ENCODER_HPP
#define PUREPHONE_ENCODER_HPP

#include <memory>

class Encoder {
public:

    virtual uint32_t Encode(int16_t *pcmData,uint32_t samplesToRead, ) = 0;

    static std::unique_ptr<Encoder> Create(const char *file);
};


#endif //PUREPHONE_ENCODER_HPP
