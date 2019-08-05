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

namespace audio {

    std::unique_ptr<Profile> Profile::Create(const Type t, std::function<int32_t()> callback, float vol, float gain) {
        std::unique_ptr<Profile> inst;

        switch (t) {
            case Type::PlaybackHeadphones:
                inst = std::make_unique<ProfilePlaybackHeadphones>(callback, vol);
                break;
            case Type::PlaybackLoudspeaker:
                inst = std::make_unique<ProfilePlaybackLoudspeaker>(callback, vol);
                break;
            case Type::RecordingBuiltInMic:
                inst = std::make_unique<ProfileRecordingOnBoardMic>(callback, gain);
                break;
            case Type::RecordingHeadset:
                inst = std::make_unique<ProfileRecordingHeadset>(callback, gain);
                break;
            default:
                break;
        }

        return inst;
    }

    Profile::Profile(const std::string &name, const Type type, const bsp::AudioDevice::Format &fmt,
                     bsp::AudioDevice::Type devType,
                     std::function<int32_t()> callback)
            : name(name), type(type), audioFormat(fmt),
              audioDeviceType(devType), dbAccessCallback(callback) {
    }


    void Profile::SetInputGain(float gain) {
        audioFormat.inputGain = gain;
        if (dbAccessCallback) {
            dbAccessCallback();
        }
    }

    void Profile::SetOutputVolume(float vol) {
        audioFormat.outputVolume = vol;
        if (dbAccessCallback) {
            dbAccessCallback();
        }
    }

    void Profile::SetInputPath(bsp::AudioDevice::InputPath path) {
        audioFormat.inputPath = path;
        if (dbAccessCallback) {
            dbAccessCallback();
        }
    }

    void Profile::SetOutputPath(bsp::AudioDevice::OutputPath path) {
        audioFormat.outputPath = path;
        if (dbAccessCallback) {
            dbAccessCallback();
        }
    }

    void Profile::SetInOutFlags(uint32_t flags) {
        audioFormat.flags = flags;
    }

    void Profile::SetSampleRate(uint32_t samplerate) {
        audioFormat.sampleRate_Hz = samplerate;
    }

}