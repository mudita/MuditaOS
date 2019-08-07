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

namespace audio {

    class Profile {
    public:

        enum class Type {
            // Profiles used only during call
            RoutingSpeakerphone,
            RoutingHeadset,
            RoutingBTHeadset,
            RoutingHeadphones,
            RoutingEarspeaker,

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

        static std::unique_ptr<Profile>
        Create(const Type t, std::function<int32_t()> callback = nullptr, float vol = 0, float gain = 0.0);

        Profile() {}

        Profile(const std::string &name, const Type type, const bsp::AudioDevice::Format &fmt,
                bsp::AudioDevice::Type devType,
                std::function<int32_t()> callback);

        void SetOutputVolume(float vol);

        void SetInputGain(float gain);

        void SetInOutFlags(uint32_t flags);

        void SetSampleRate(uint32_t samplerate);

        void SetOutputPath(bsp::AudioDevice::OutputPath path);

        void SetInputPath(bsp::AudioDevice::InputPath path);

        float GetOutputVolume() const { return audioFormat.outputVolume; }

        float GetInputGain() const { return audioFormat.inputGain; }

        uint32_t GetSampleRate(){return audioFormat.sampleRate_Hz;}

        uint32_t GetInOutFlags(){return audioFormat.flags;}

        bsp::AudioDevice::OutputPath GetOutputPath() const { return audioFormat.outputPath; }

        bsp::AudioDevice::InputPath GetInputPath() const { return audioFormat.inputPath; }

        bsp::AudioDevice::Type GetAudioDeviceType() const { return audioDeviceType; }

        bsp::AudioDevice::Format GetAudioFormat(){return audioFormat;}

        const std::string &GetName() const { return name; }

        Type GetType() { return type; }


    protected:
        bsp::AudioDevice::Format audioFormat;
        bsp::AudioDevice::Type audioDeviceType = bsp::AudioDevice::Type::Audiocodec;

        std::string name;
        Type type = Type::Idle;

        std::function<int32_t()> dbAccessCallback = nullptr;


    };

}

#endif //PUREPHONE_PROFILE_HPP
