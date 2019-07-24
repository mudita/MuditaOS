/*
 *  @file Profile.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 23.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "Profile.hpp"

Profile::Profile(const std::string& name,const Type type, uint32_t vol, float gain, uint32_t outPath, uint32_t inPath,
                 bsp::AudioDevice::Type devType, std::function<int32_t()> callback)
        : name(name),type(type), outputVolume(vol), inputGain(gain), outputPath(outPath), inputPath(inPath),
          audioDeviceType(devType), dbAccessCallback(callback) {
}


void Profile::SetInputGain(float gain) {
    inputGain = gain;
    dbAccessCallback();
}

void Profile::SetOutputVolume(uint32_t vol) {
    outputVolume = vol;
    dbAccessCallback();
}

void Profile::SetInputPath(int8_t path) {
    inputPath = path;
    dbAccessCallback();
}

void Profile::SetOutputPath(uint32_t path) {
    outputPath = path;
    dbAccessCallback();
}