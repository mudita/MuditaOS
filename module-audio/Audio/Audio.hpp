// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-bluetooth/ServiceBluetoothCommon.hpp>

#include "AudioCommon.hpp"
#include "decoder/Decoder.hpp"
#include "Operation/Operation.hpp"

#include <bitset>
#include <functional>
#include <memory>
#include <optional>

namespace audio
{

    class Audio
    {
        enum class Muted : bool
        {
            True,
            False
        };

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

        [[nodiscard]] auto IsMuted() const noexcept
        {
            return muted == Muted::True;
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

        Muted muted = Muted::False;
        AudioSinkState audioSinkState;

        std::shared_ptr<BluetoothStreamData> btData;

        State currentState = State::Idle;
        std::unique_ptr<Operation> currentOperation;

        AudioServiceMessage::Callback serviceCallback;
    };

} // namespace audio
