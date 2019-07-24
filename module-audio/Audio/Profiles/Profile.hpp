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

    Profile(const std::string& name,const Type type, uint32_t vol, int8_t gain, uint32_t outPath, uint32_t inPath,bsp::AudioDevice::Type devType,
            std::function<int32_t()> callback);


    void SetOutputVolume(uint32_t vol);

    void SetInputGain(int8_t gain);

    void SetOutputPath(uint32_t path);

    void SetInputPath(int8_t path);

    uint32_t GetOutputVolume() const { return outputVolume; }

    int8_t GetInputGain() const { return inputGain; }

    uint32_t GetOutputPath() const{return outputPath;}

    uint32_t GetInputPath() const {return inputPath;}

    bsp::AudioDevice::Type GetAudioDeviceType() const {return audioDeviceType;}

    const std::string& GetName() const {return name;}


protected:
    uint32_t outputVolume;
    int8_t inputGain;
    uint32_t outputPath;
    uint32_t inputPath;

    const std::string name;
    const Type type;

    std::function<int32_t()> dbAccessCallback;

    const bsp::AudioDevice::Type audioDeviceType;
};


#endif //PUREPHONE_PROFILE_HPP
