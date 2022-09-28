// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <functional>
#include <optional>

#include "Audio/AudioCommon.hpp"
#include "Audio/AudioDeviceFactory.hpp"
#include "Audio/AudioPlatform.hpp"
#include "Audio/ServiceObserver.hpp"
#include "Audio/encoder/Encoder.hpp"
#include "Audio/Profiles/Profile.hpp"

namespace audio
{
    class Operation
    {
      public:
        explicit Operation(AudioServiceMessage::Callback callback,
                           const PlaybackType &playbackType = PlaybackType::None)
            : playbackType(playbackType), serviceCallback(callback), observer(serviceCallback)
        {
            factory = AudioPlatform::GetDeviceFactory();
            factory->setObserver(&observer);
        }

        enum class State
        {
            Idle,
            Active,
            Paused
        };

        enum class Type
        {
            Idle,
            Playback,
            Recorder,
            Router
        };

        [[nodiscard]] static inline auto c_str(Type type) -> const char *
        {
            switch (type) {
            case Type::Idle:
                return "Idle";
            case Type::Playback:
                return "Playback";
            case Type::Recorder:
                return "Recorder";
            case Type::Router:
                return "Router ";
            }
            return "";
        }

        virtual ~Operation() = default;

        static std::unique_ptr<Operation> Create(Type t,
                                                 const char *fileName                   = "",
                                                 const audio::PlaybackType &operations  = audio::PlaybackType::None,
                                                 AudioServiceMessage::Callback callback = nullptr);

        virtual audio::RetCode Start(audio::Token token)             = 0;
        virtual audio::RetCode Stop()                                = 0;
        virtual audio::RetCode Pause()                               = 0;
        virtual audio::RetCode Resume()                              = 0;
        virtual audio::RetCode SendEvent(std::shared_ptr<Event> evt) = 0;
        virtual audio::RetCode SetOutputVolume(float vol)            = 0;
        virtual audio::RetCode SetInputGain(float gain)              = 0;

        virtual Position GetPosition() = 0;

        Volume GetOutputVolume() const
        {
            return (currentProfile != nullptr) ? currentProfile->GetOutputVolume() : Volume{};
        }

        Gain GetInputGain() const
        {
            return currentProfile->GetInputGain();
        }

        State GetState() const
        {
            return state;
        }

        const std::shared_ptr<Profile> GetProfile() const
        {
            return currentProfile;
        }

        audio::PlaybackType GetPlaybackType() const noexcept
        {
            return playbackType;
        }

        const audio::Token &GetToken() const noexcept
        {
            return operationToken;
        }

        Type GetOperationType() const noexcept
        {
            return opType;
        }

        std::string GetFilePath() const noexcept
        {
            return filePath;
        }
        std::optional<Profile::Type> GetPriorityProfile() const;
        /**
         * @brief Switches operation to priority profile.
         */
        virtual audio::RetCode SwitchToPriorityProfile();
        /**
         * @brief Switches operation to priority profile.
         *
         * @param playbackType if it's callringtone and bluetooth a2dp is used then
         * ignore priorities and change profile to the earpeaker. Not needed otherwise.
         */
        virtual audio::RetCode SwitchToPriorityProfile(audio::PlaybackType playbackType);

      protected:
        struct SupportedProfile
        {
            SupportedProfile(std::shared_ptr<Profile> profile, bool isAvailable)
                : profile(std::move(profile)), isAvailable(isAvailable)
            {}

            std::shared_ptr<Profile> profile;
            bool isAvailable;
        };

        std::shared_ptr<Profile> currentProfile;
        std::shared_ptr<AudioDevice> audioDevice;
        std::vector<SupportedProfile> supportedProfiles;
        std::unique_ptr<AudioDeviceFactory> factory;

        State state = State::Idle;
        audio::Token operationToken;
        Type opType = Type::Idle;
        std::string filePath;
        audio::PlaybackType playbackType = audio::PlaybackType::None;

        AudioServiceMessage::Callback serviceCallback;
        ServiceObserver observer;

        std::function<int32_t(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer)>
            audioCallback = nullptr;

        void SetProfileAvailability(std::vector<Profile::Type> profiles, bool available);
        void AddProfile(const Profile::Type &profile, const PlaybackType &playback, bool isAvailable);

        virtual audio::RetCode SwitchProfile(const Profile::Type type) = 0;
        std::shared_ptr<Profile> GetProfile(const Profile::Type type);

        std::shared_ptr<AudioDevice> CreateDevice(const Profile &profile);
        std::shared_ptr<AudioDevice> createCellularAudioDevice();
    };

} // namespace audio
