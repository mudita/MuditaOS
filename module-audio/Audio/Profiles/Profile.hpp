/*
 *  @file Profile.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 23.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_PROFILE_HPP
#define PUREPHONE_PROFILE_HPP

#include <memory>
#include <functional>

#include "bsp/audio/bsp_audio.hpp"

class Profile {
public:

    enum class Type {
        // Profiles used only during call
        CallSpeakerphone,
        CallHeadset,
        CallBTHeadset,
        CallHeadphones,
        CallEarspeaker,

        // Recording profiles
        RecordingBuiltInMic,
        RecordingHeadset,
        RecordingBTHeadset,

        // Profiles used by music player
        PlaybackLoudspeaker,
        PlaybackHeadphones,
        PlaybackBTA2DP,

        // Profiles used by system sounds
        SystemSoundLoudspeaker,
        SystemSoundHeadphones,
        SystemSoundBTA2DP,

        Idle,

    };

    static std::unique_ptr<Profile> Create(const Type t,std::function<int32_t()> callback=nullptr,float vol = 0,float gain = 0.0);

    Profile(){}

    Profile(const std::string& name,const Type type, float vol, float gain, uint32_t outPath, uint32_t inPath,bsp::AudioDevice::Type devType,
            std::function<int32_t()> callback);

    void SetOutputVolume(uint32_t vol);

    void SetInputGain(float gain);

    void SetOutputPath(uint32_t path);

    void SetInputPath(int8_t path);

    float GetOutputVolume() const { return outputVolume; }

    float GetInputGain() const { return inputGain; }

    uint32_t GetOutputPath() const{return outputPath;}

    uint32_t GetInputPath() const {return inputPath;}

    bsp::AudioDevice::Type GetAudioDeviceType() const {return audioDeviceType;}

    const std::string& GetName() const {return name;}

    Type GetType(){return type;}


protected:
    float outputVolume=1.0;
    float inputGain=1.0;
    uint32_t outputPath=0;
    uint32_t inputPath=0;

    std::string name;
    Type type=Type::Idle;

    std::function<int32_t()> dbAccessCallback=nullptr;

    bsp::AudioDevice::Type audioDeviceType=bsp::AudioDevice::Type ::Audiocodec;
};


#endif //PUREPHONE_PROFILE_HPP
