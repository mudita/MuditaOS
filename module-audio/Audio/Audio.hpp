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

        Audio(std::function<int32_t(AudioEvents event)> asyncCallback,
              std::function<uint32_t(const std::string &path, const uint32_t &defaultValue)> dbCallback);

        // Events
        audio::RetCode SendEvent(const Operation::Event evt, const EventData *data = nullptr);

        // utilities
        Position GetPosition();

        State GetCurrentState() const
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

        // TODO:M.P Set/Get inputGain/outputVolume for each profile

        // Operations
        audio::RetCode Start(Operation::Type op, const char *fileName = "");

        audio::RetCode Stop();

        audio::RetCode Pause();

        audio::RetCode Resume();

      private:
        bool headphonesInserted = false;
        State currentState = State::Idle;
        std::unique_ptr<Operation> currentOperation;

        std::function<int32_t(AudioEvents event)> asyncCallback = nullptr;
        std::function<uint32_t(const std::string &path, const uint32_t &defaultValue)> dbCallback = nullptr;
    };

} // namespace audio
