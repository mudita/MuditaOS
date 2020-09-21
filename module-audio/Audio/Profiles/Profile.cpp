#include "Profile.hpp"

#include "ProfileRecordingOnBoardMic.hpp"
#include "ProfileRecordingHeadset.hpp"

#include "ProfilePlaybackHeadphones.hpp"
#include "ProfilePlaybackLoudspeaker.hpp"

#include "ProfileRoutingEarspeaker.hpp"
#include "ProfileRoutingHeadset.hpp"
#include "ProfileRoutingSpeakerphone.hpp"

namespace audio
{

    std::unique_ptr<Profile> Profile::Create(const Type t, std::function<int32_t()> callback, float vol, float gain)
    {
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
        case Type::RoutingHeadset:
            inst = std::make_unique<ProfileRoutingHeadset>(callback, vol, gain);
            break;
        case Type::RoutingSpeakerphone:
            inst = std::make_unique<ProfileRoutingSpeakerphone>(callback, vol, gain);
            break;
        case Type::RoutingEarspeaker:
            inst = std::make_unique<ProfileRoutingEarspeaker>(callback, vol, gain);
            break;
        default:
            break;
        }

        return inst;
    }

    Profile::Profile(const std::string &name,
                     const Type type,
                     const bsp::AudioDevice::Format &fmt,
                     bsp::AudioDevice::Type devType,
                     std::function<int32_t()> callback)
        : audioFormat(fmt), audioDeviceType(devType), name(name), type(type), dbAccessCallback(callback)
    {}

    void Profile::SetInputGain(float gain)
    {
        audioFormat.inputGain = gain;
        if (dbAccessCallback) {
            dbAccessCallback();
        }
    }

    void Profile::SetOutputVolume(float vol)
    {
        audioFormat.outputVolume = vol;
        if (dbAccessCallback) {
            dbAccessCallback();
        }
    }

    void Profile::SetInputPath(bsp::AudioDevice::InputPath path)
    {
        audioFormat.inputPath = path;
        if (dbAccessCallback) {
            dbAccessCallback();
        }
    }

    void Profile::SetOutputPath(bsp::AudioDevice::OutputPath path)
    {
        audioFormat.outputPath = path;
        if (dbAccessCallback) {
            dbAccessCallback();
        }
    }

    void Profile::SetInOutFlags(uint32_t flags)
    {
        audioFormat.flags = flags;
    }

    void Profile::SetSampleRate(uint32_t samplerate)
    {
        audioFormat.sampleRate_Hz = samplerate;
    }

    const std::string str(const Profile::Type &profileType)
    {
        switch (profileType) {
        case Profile::Type::RoutingSpeakerphone: {
            return "RoutingSpeakerphone";
        }
        case Profile::Type::RoutingHeadset: {
            return "RoutingHeadset";
        }
        case Profile::Type::RoutingBTHeadset: {
            return "RoutingBTHeadset";
        }
        case Profile::Type::RoutingHeadphones: {
            return "RoutingHeadphones";
        }
        case Profile::Type::RoutingEarspeaker: {
            return "RoutingEarspeaker";
        }
        case Profile::Type::RecordingBuiltInMic: {
            return "RecordingBuiltInMic";
        }
        case Profile::Type::RecordingHeadset: {
            return "RecordingHeadset";
        }
        case Profile::Type::RecordingBTHeadset: {
            return "RecordingBTHeadset";
        }
        case Profile::Type::PlaybackLoudspeaker: {
            return "PlaybackLoudspeaker";
        }
        case Profile::Type::PlaybackHeadphones: {
            return "PlaybackHeadphones";
        }
        case Profile::Type::PlaybackBTA2DP: {
            return "PlaybackBTA2DP";
        }
        case Profile::Type::SystemSoundLoudspeaker: {
            return "SystemSoundLoudspeaker";
        }
        case Profile::Type::SystemSoundHeadphones: {
            return "SystemSoundHeadphones";
        }
        case Profile::Type::SystemSoundBTA2DP: {
            return "SystemSoundBTA2DP";
        }
        case Profile::Type::Idle: {
            return "";
        }
        }
        return "";
    }
} // namespace audio
