#pragma once

#include <memory>
#include <optional>
#include <functional>

#include "Audio/AudioCommon.hpp"

#include "Audio/encoder/Encoder.hpp"
#include "Audio/Profiles/Profile.hpp"

namespace audio
{
    class EventData
    {
      public:
        ~EventData()
        {}
    };

    class Operation
    {
      public:
        Operation(const bool &isInitialized               = false,
                  const audio::PlaybackType &playbackType = audio::PlaybackType::None)
            : isInitialized{isInitialized}, playbackType{playbackType}
        {}

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

        enum class Event
        {
            HeadphonesPlugin,
            HeadphonesUnplug,
            BTHeadsetOn,
            BTHeadsetOff,
            BTA2DPOn,
            BTA2DPOff,
            CallMute,
            CallUnmute,
            CallSpeakerphoneOn,
            CallSpeakerphoneOff,
        };

        virtual ~Operation() = default;

        static std::optional<std::unique_ptr<Operation>> Create(
            Type t,
            const char *fileName                  = "",
            const audio::PlaybackType &operations = audio::PlaybackType::None,
            std::function<uint32_t(const std::string &path, const uint32_t &defaultValue)> dbCallback = nullptr);

        virtual audio::RetCode Start(audio::AsyncCallback callback, audio::Token token) = 0;

        virtual audio::RetCode Stop() = 0;

        virtual audio::RetCode Pause() = 0;

        virtual audio::RetCode Resume() = 0;

        virtual audio::RetCode SendEvent(const Event evt, const EventData *data = nullptr) = 0;

        virtual audio::RetCode SetOutputVolume(float vol) = 0;

        virtual audio::RetCode SetInputGain(float gain) = 0;

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

        const std::shared_ptr<const Profile> GetProfile() const
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

      protected:
        std::shared_ptr<Profile> currentProfile;
        std::vector<std::shared_ptr<Profile>> availableProfiles;
        State state = State::Idle;
        audio::AsyncCallback eventCallback;

        audio::Token operationToken;
        Type opType = Type::Idle;
        std::string filePath;

        bool isInitialized               = false;
        audio::PlaybackType playbackType = audio::PlaybackType::None;

        virtual audio::RetCode SwitchProfile(const Profile::Type type) = 0;

        std::optional<std::shared_ptr<Profile>> GetProfile(const Profile::Type type);

        std::function<int32_t(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer)>
            audioCallback = nullptr;
    };

} // namespace audio
