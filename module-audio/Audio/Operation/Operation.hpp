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

        enum class Event
        {
            HeadphonesPlugin,
            HeadphonesUnplug,
            BTHeadsetOn,
            BTHeadsetOff,
            BTA2DPOn,
            BTA2DPOff,
            StartCallRecording,
            StopCallRecording,
            CallMute,
            CallUnmute,
            CallSpeakerphoneOn,
            CallSpeakerphoneOff,
        };

        virtual ~Operation()
        {}

        static std::optional<std::unique_ptr<Operation>> Create(Type t, const char *fileName);

        virtual int32_t Start(std::function<int32_t(AudioEvents event)> callback) = 0;

        virtual int32_t Stop() = 0;

        virtual int32_t Pause() = 0;

        virtual int32_t Resume() = 0;

        virtual int32_t SendEvent(const Event evt, const EventData *data = nullptr) = 0;

        virtual int32_t SetOutputVolume(float vol) = 0;

        virtual int32_t SetInputGain(float gain) = 0;

        virtual Position GetPosition() = 0;

        Volume GetOutputVolume()
        {
            return currentProfile != nullptr ? currentProfile->GetOutputVolume() : 0; // TODO: create const for invalid
        }

        Gain GetInputGain()
        {
            return currentProfile != nullptr ? currentProfile->GetInputGain() : 0; // TODO: create const for invalid
        }

        State GetState()
        {
            return state;
        }

        const Profile *GetProfile()
        {
            return currentProfile;
        }

      protected:
        Profile *currentProfile = nullptr;
        std::vector<std::unique_ptr<Profile>> availableProfiles;
        State state                                             = State::Idle;
        std::function<int32_t(AudioEvents event)> eventCallback = nullptr;

        bool isInitialized = false;

        virtual int32_t SwitchProfile(const Profile::Type type) = 0;

        std::optional<Profile *> GetProfile(const Profile::Type type);

        std::function<int32_t(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer)>
            audioCallback = nullptr;
    };

} // namespace audio
