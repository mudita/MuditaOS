// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Audio/AudioDevice.hpp>
#include <Audio/codec.hpp>

#include <memory>
#include <functional>
#include <string>

namespace audio
{
    using Volume        = uint32_t;
    using Gain          = uint32_t;
    using Position      = float;
    using Vibrate       = bool;
    using EnableSound   = bool;
    using IsSystemSound = bool;

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
            RoutingBluetoothHFP,

            // Recording profiles
            RecordingBuiltInMic,
            RecordingHeadphones,
            RecordingBluetoothHSP,
            RecordingBluetoothHFP,

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

        void SetOutputPath(audio::codec::OutputPath path);

        void SetInputPath(audio::codec::InputPath path);

        Volume GetOutputVolume() const
        {
            return audioConfiguration.outputVolume;
        }

        Gain GetInputGain() const
        {
            return audioConfiguration.inputGain;
        }

        uint32_t GetSampleRate()
        {
            return audioConfiguration.sampleRate_Hz;
        }

        uint32_t GetInOutFlags()
        {
            return audioConfiguration.flags;
        }

        audio::codec::OutputPath GetOutputPath() const
        {
            return audioConfiguration.outputPath;
        }

        audio::codec::InputPath GetInputPath() const
        {
            return audioConfiguration.inputPath;
        }

        AudioDevice::Type GetAudioDeviceType() const
        {
            return audioDeviceType;
        }

        [[deprecated]] audio::codec::Configuration GetAudioConfiguration() const
        {
            return audioConfiguration;
        }

        auto getAudioFormat() const noexcept
        {
            auto isStereo =
                (audioConfiguration.flags & static_cast<std::uint32_t>(audio::codec::Flags::OutputStereo)) != 0 ||
                (audioConfiguration.flags & static_cast<std::uint32_t>(audio::codec::Flags::InputStereo)) != 0;
            auto channels = isStereo ? 2U : 1U;
            return AudioFormat(audioConfiguration.sampleRate_Hz, audioConfiguration.bitWidth, channels);
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
        Profile(const std::string &name,
                const Type type,
                const audio::codec::Configuration &fmt,
                AudioDevice::Type devType);

        audio::codec::Configuration audioConfiguration;
        AudioDevice::Type audioDeviceType = AudioDevice::Type::Audiocodec;

        std::string name;
        Type type = Type::Idle;
    };

    [[nodiscard]] const std::string str(const Profile::Type &profileType);
} // namespace audio
