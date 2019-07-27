/*
 *  @file Profile.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 23.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "Profile.hpp"

#include "ProfileRecordingOnBoardMic.hpp"
#include "ProfileRecordingHeadset.hpp"
#include "ProfilePlaybackHeadphones.hpp"
#include "ProfilePlaybackLoudspeaker.hpp"

std::unique_ptr<Profile> Profile::Create(const Type t,std::function<int32_t()> callback, float vol, float gain) {
    std::unique_ptr<Profile> inst;

    switch(t){
        case Type ::PlaybackHeadphones:
            inst = std::make_unique<ProfilePlaybackHeadphones>(callback,vol);
            break;
        case Type::PlaybackLoudspeaker:
            inst = std::make_unique<ProfilePlaybackLoudspeaker>(callback,vol);
            break;
        case Type::RecordingBuiltInMic:
            inst = std::make_unique<ProfileRecordingOnBoardMic>(callback,gain);
            break;
        case Type::RecordingHeadset:
            inst = std::make_unique<ProfileRecordingHeadset>(callback,gain);
            break;
        default:
            break;
    }

    return inst;
}

Profile::Profile(const std::string& name,const Type type, float vol, float gain, uint32_t outPath, uint32_t inPath,
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