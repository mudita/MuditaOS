// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

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
            if (audioSinkState.isConnected(EventType::JackState)) {
                return Profile::Type::PlaybackHeadphones;
            }
            if (audioSinkState.isConnected(EventType::BlutoothA2DPDeviceState)) {
                return Profile::Type::PlaybackBluetoothA2DP;
            }
            return Profile::Type::PlaybackLoudspeaker;
        }

        // Operations
        virtual audio::RetCode Start(Operation::Type op,
                                     audio::Token token                      = audio::Token::MakeBadToken(),
                                     const std::string &filePath             = "",
                                     const audio::PlaybackType &playbackType = audio::PlaybackType::None);

        virtual audio::RetCode Start();
        virtual audio::RetCode Stop();
        virtual audio::RetCode Pause();
        virtual audio::RetCode Resume();
        virtual audio::RetCode Mute();

      protected:
        AudioSinkState audioSinkState;

      private:
        void SendUpdateEventsToCurrentOperation();
        /**
         * @brief Sends update to the current operation and switches to priority profile.
         */
        void UpdateProfiles();
        /**
         * @brief Sends update to the current operation and switches to priority profile.
         *
         * @param playbackType if it's callringtone and bluetooth a2dp is used then
         * ignore priorities and change profile to the earpeaker. Not needed otherwise.
         */
        void UpdateProfiles(audio::PlaybackType playbackType);

        Muted muted = Muted::False;

        State currentState = State::Idle;
        std::unique_ptr<Operation> currentOperation;

        AudioServiceMessage::Callback serviceCallback;
    };

} // namespace audio
