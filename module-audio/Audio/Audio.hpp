// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <optional>
#include <functional>
#include <bitset>

#include <service-bluetooth/ServiceBluetoothCommon.hpp>

#include "AudioCommon.hpp"
#include "Stream.hpp"
#include "Operation/Operation.hpp"
#include "decoder/Decoder.hpp"

namespace audio
{

    class Audio
    {
      public:
        enum class State
        {
            Idle,
            Playback,
            Recording,
            Routing,
        };

        Audio(AudioServiceMessage::Callback callback);

        virtual ~Audio() = default;

        // Events
        audio::RetCode SendEvent(std::shared_ptr<Event> evt);

        // utilities
        Position GetPosition();

        virtual State GetCurrentState() const
        {
            return currentState;
        }

        static std::optional<Tags> GetFileTags(const char *filename);

        // Range 0-1
        audio::RetCode SetOutputVolume(Volume vol);

        // Range 0-10
        audio::RetCode SetInputGain(Gain gain);

        Volume GetOutputVolume()
        {
            return currentOperation->GetOutputVolume();
        }

        Gain GetInputGain()
        {
            return currentOperation->GetInputGain();
        }

        const Operation &GetCurrentOperation() const
        {
            // currentOperation always exists - null pattern design
            return *(currentOperation.get());
        }

        virtual audio::PlaybackType GetCurrentOperationPlaybackType() const
        {
            return GetCurrentOperation().GetPlaybackType();
        }

        virtual Operation::State GetCurrentOperationState() const
        {
            return GetCurrentOperation().GetState();
        }

        audio::Profile::Type GetPriorityPlaybackProfile() const
        {
            if (audioSinkState.isConnected(EventType::BlutoothA2DPDeviceState)) {
                return Profile::Type::PlaybackBluetoothA2DP;
            }
            if (audioSinkState.isConnected(EventType::JackState)) {
                return Profile::Type::PlaybackHeadphones;
            }
            return Profile::Type::PlaybackLoudspeaker;
        }

        // Operations
        virtual audio::RetCode Start(Operation::Type op,
                                     audio::Token token                      = audio::Token::MakeBadToken(),
                                     const char *fileName                    = "",
                                     const audio::PlaybackType &playbackType = audio::PlaybackType::None);

        virtual audio::RetCode Start();

        virtual audio::RetCode Stop();

        virtual audio::RetCode Pause();

        virtual audio::RetCode Resume();

        virtual audio::RetCode Mute();

      private:
        void UpdateProfiles();
        AudioSinkState audioSinkState;

        std::shared_ptr<BluetoothStreamData> btData;

        State currentState = State::Idle;
        std::unique_ptr<Operation> currentOperation;

        AudioServiceMessage::Callback serviceCallback;

        // for efficiency multiple of 24 and 32 (max audio samples size)
        static constexpr auto defaultAudioStreamBlockSize = 2048;
        StandardStreamAllocator allocatorOut;
        Stream dataStreamOut{allocatorOut, defaultAudioStreamBlockSize};

        StandardStreamAllocator allocatorIn;
        Stream dataStreamIn{allocatorIn, defaultAudioStreamBlockSize};
    };

} // namespace audio
