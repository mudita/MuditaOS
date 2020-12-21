// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Profile.hpp"

#include "ProfileIdle.hpp"

#include "ProfileRecordingOnBoardMic.hpp"
#include "ProfileRecordingHeadphones.hpp"
#include "ProfileRecordingBluetoothHSP.hpp"

#include "ProfilePlaybackHeadphones.hpp"
#include "ProfilePlaybackLoudspeaker.hpp"

#include "ProfileRoutingEarspeaker.hpp"
#include "ProfileRoutingHeadphones.hpp"
#include "ProfileRoutingLoudspeaker.hpp"
#include "ProfileRoutingBluetoothHSP.hpp"

#include "ProfilePlaybackBluetoothA2DP.hpp"

#include <Utils.hpp>

namespace audio
{

    std::unique_ptr<Profile> Profile::Create(const Type t,
                                             std::function<int32_t()> callback,
                                             std::optional<Volume> vol,
                                             std::optional<Gain> gain)
    {
        std::unique_ptr<Profile> inst;

        switch (t) {
        case Type::PlaybackHeadphones:
            assert(vol);
            inst = std::make_unique<ProfilePlaybackHeadphones>(callback, vol.value());
            break;
        case Type::PlaybackLoudspeaker:
            assert(vol);
            inst = std::make_unique<ProfilePlaybackLoudspeaker>(callback, vol.value());
            break;
        case Type::PlaybackBluetoothA2DP:
            assert(vol);
            inst = std::make_unique<ProfilePlaybackBluetoothA2DP>(callback, vol.value());
            break;
        case Type::RecordingBuiltInMic:
            assert(gain);
            inst = std::make_unique<ProfileRecordingOnBoardMic>(callback, gain.value());
            break;
        case Type::RecordingHeadphones:
            assert(gain);
            inst = std::make_unique<ProfileRecordingHeadphones>(callback, gain.value());
            break;
        case Type::RecordingBluetoothHSP:
            assert(gain);
            inst = std::make_unique<ProfileRecordingBluetoothHSP>(callback, gain.value());
            break;
        case Type::RoutingHeadphones:
            assert(gain && vol);
            inst = std::make_unique<ProfileRoutingHeadphones>(callback, vol.value(), gain.value());
            break;
        case Type::RoutingLoudspeaker:
            assert(gain && vol);
            inst = std::make_unique<ProfileRoutingLoudspeaker>(callback, vol.value(), gain.value());
            break;
        case Type::RoutingEarspeaker:
            assert(gain && vol);
            inst = std::make_unique<ProfileRoutingEarspeaker>(callback, vol.value(), gain.value());
            break;
        case Type::RoutingBluetoothHSP:
            assert(gain && vol);
            inst = std::make_unique<ProfileRoutingBluetoothHSP>(callback, vol.value(), gain.value());
            break;
        case Type::Idle:
            inst = std::make_unique<ProfileIdle>();
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

    void Profile::SetInputGain(Gain gain)
    {
        audioFormat.inputGain = gain;
        if (dbAccessCallback) {
            dbAccessCallback();
        }
    }

    void Profile::SetOutputVolume(Volume vol)
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
        if (profileType == Profile::Type::Idle) {
            return "";
        }
        return utils::enumToString(profileType);
    }
} // namespace audio
