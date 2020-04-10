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

        Audio(std::function<int32_t(AudioEvents event)> asyncCallback);

        // Events
        int32_t SendEvent(const Operation::Event evt, const EventData *data = nullptr);

        // utilities
        Position GetPosition();

        State GetCurrentState() const
        {
            return currentState;
        }

        std::optional<Tags> GetFileTags(const char *filename);

        // Range 0-1
        int32_t SetOutputVolume(Volume vol);

        // Range 0-10
        int32_t SetInputGain(Gain gain);

        Volume GetOutputVolume()
        {
            return currentOperation != nullptr ? currentOperation->GetOutputVolume() : invalidVolume;
        }

        Gain GetInputGain()
        {
            return currentOperation != nullptr ? currentOperation->GetInputGain() : invalidGain;
        }

        // TODO:M.P Set/Get inputGain/outputVolume for each profile

        // Operations
        int32_t Start(Operation::Type op, const char *fileName = "");

        int32_t Stop();

        int32_t Pause();

        int32_t Resume();

      private:
        State currentState = State::Idle;
        std::unique_ptr<Operation> currentOperation;

        std::function<int32_t(AudioEvents event)> asyncCallback = nullptr;
    };

} // namespace audio
