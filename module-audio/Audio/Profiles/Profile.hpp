// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Audio/AudioDevice.hpp"

#include <memory>
#include <functional>
#include <string>

namespace audio
{
    using Volume      = uint32_t;
    using Gain        = uint32_t;
    using Position    = float;
    using Vibrate     = bool;
    using EnableSound = bool;

    class Profile
    {
      public:
        enum class Type
        {
            // Profiles used only during call
            RoutingLoudspeaker,
            RoutingHeadphones,
            RoutingEarspeaker,
            RoutingBluetoothHSP,

            // Recording profiles
            RecordingBuiltInMic,
            RecordingHeadphones,
            RecordingBluetoothHSP,

            // Profiles used by music player
            PlaybackLoudspeaker,
            PlaybackHeadphones,
            PlaybackBluetoothA2DP,

            Idle,

        };

        static std::unique_ptr<Profile> Create(const Type t,
                                               std::optional<Volume> vol = 0,
                                               std::optional<Gain> gain  = 0);

        void SetOutputVolume(Volume vol);

        void SetInputGain(Gain gain);

        void SetInOutFlags(uint32_t flags);

        void SetSampleRate(uint32_t samplerate);

        void SetOutputPath(AudioDevice::OutputPath path);

        void SetInputPath(AudioDevice::InputPath path);

        Volume GetOutputVolume() const
        {
            return audioFormat.outputVolume;
        }

        Gain GetInputGain() const
        {
            return audioFormat.inputGain;
        }

        uint32_t GetSampleRate()
        {
            return audioFormat.sampleRate_Hz;
        }

        uint32_t GetInOutFlags()
        {
            return audioFormat.flags;
        }

        AudioDevice::OutputPath GetOutputPath() const
        {
            return audioFormat.outputPath;
        }

        AudioDevice::InputPath GetInputPath() const
        {
            return audioFormat.inputPath;
        }

        AudioDevice::Type GetAudioDeviceType() const
        {
            return audioDeviceType;
        }

        AudioDevice::Format GetAudioFormat()
        {
            return audioFormat;
        }

        const std::string &GetName() const
        {
            return name;
        }

        Type GetType() const
        {
            return type;
        }

      protected:
        Profile(const std::string &name, const Type type, const AudioDevice::Format &fmt, AudioDevice::Type devType);

        AudioDevice::Format audioFormat{};
        AudioDevice::Type audioDeviceType = AudioDevice::Type::Audiocodec;

        std::string name;
        Type type = Type::Idle;
    };

    [[nodiscard]] const std::string str(const Profile::Type &profileType);
} // namespace audio
