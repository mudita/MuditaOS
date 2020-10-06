#pragma once

#include <memory>
#include <optional>
#include <functional>

#include "AudioCommon.hpp"
#include "Operation/Operation.hpp"
#include "decoder/decoder.hpp"

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

        Audio(AsyncCallback asyncCallback, DbCallback dbCallback);

        virtual ~Audio() = default;

        // Events
        audio::RetCode SendEvent(const Operation::Event evt, const EventData *data = nullptr);

        // utilities
        Position GetPosition();

        virtual State GetCurrentState() const
        {
            return currentState;
        }

        std::optional<Tags> GetFileTags(const char *filename);

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

        const Operation *GetCurrentOperation() const
        {
            return currentOperation.get();
        }

        virtual std::optional<audio::PlaybackType> GetCurrentOperationPlaybackType() const
        {
            if (currentOperation.get()) {
                return currentOperation.get()->GetPlaybackType();
            }
            return std::nullopt;
        }

        virtual std::optional<Operation::State> GetCurrentOperationState() const
        {
            if (currentOperation.get()) {
                return currentOperation.get()->GetState();
            }
            return std::nullopt;
        }

        [[nodiscard]] inline bool GetHeadphonesInserted() const
        {
            return headphonesInserted;
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
        bool headphonesInserted = false;
        State currentState = State::Idle;
        std::unique_ptr<Operation> currentOperation;

        AsyncCallback asyncCallback;
        DbCallback dbCallback;
    };

} // namespace audio
