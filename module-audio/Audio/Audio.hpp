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
        static bool headphonesInserted;
        State currentState = State::Idle;
        std::unique_ptr<Operation> currentOperation;

        AsyncCallback asyncCallback;
        DbCallback dbCallback;
    };

} // namespace audio
