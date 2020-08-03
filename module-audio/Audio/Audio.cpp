#include "Audio.hpp"
#include "Operation/Operation.hpp"
#include <log/log.hpp>
#include <Utils.hpp>

namespace audio
{

    Audio::Audio(std::function<int32_t(AudioEvents event)> asyncCallback)
        : currentOperation(), asyncCallback(asyncCallback)
    {

        auto ret = Operation::Create(Operation::Type::Idle, "");
        if (ret) {
            currentOperation = std::move(ret.value());
        }
    }

    Position Audio::GetPosition()
    {
        return currentOperation != nullptr ? currentOperation->GetPosition() : -1; // TODO: need to be fixed
    }

    std::optional<Tags> Audio::GetFileTags(const char *filename)
    {
        auto ret = decoder::Create(filename);
        if (ret == nullptr) {
            return {};
        }
        else {
            return *ret->fetchTags();
        };
    }

    audio::RetCode Audio::SendEvent(const Operation::Event evt, const EventData *data)
    {
        switch (evt) {
        case Operation::Event::HeadphonesPlugin:
            headphonesInserted = true;
            break;
        case Operation::Event::HeadphonesUnplug:
            headphonesInserted = false;
            break;
        default:
            break;
        }

        return currentOperation != nullptr ? currentOperation->SendEvent(evt, data) : RetCode::OperationNotSet;
    }

    audio::RetCode Audio::SetOutputVolume(Volume vol)
    {
        auto volToSet = vol;
        if (vol > 1) {
            volToSet = 1;
        }
        if (vol < 0) {
            volToSet = 0;
        }

        return currentOperation != nullptr ? currentOperation->SetOutputVolume(volToSet) : RetCode::OperationNotSet;
    }

    audio::RetCode Audio::SetInputGain(Gain gain)
    {
        auto gainToSet = gain;
        if (gain > 10) {
            gainToSet = 10.0;
        }
        if (gain < 0) {
            gainToSet = 0;
        }
        return currentOperation != nullptr ? currentOperation->SetInputGain(gainToSet) : RetCode::OperationNotSet;
    }

    audio::RetCode Audio::Start(Operation::Type op, const char *fileName)
    {

        auto ret = Operation::Create(op, fileName);
        if (ret) {

            switch (op) {
            case Operation::Type::Playback:
                currentState = State::Playback;
                break;
            case Operation::Type::Recorder:
                currentState = State::Recording;
                break;
            case Operation::Type::Router:
                currentState = State::Routing;
                break;
            case Operation::Type::Idle:
                break;
            }
            currentOperation = std::move(ret.value());

            if (headphonesInserted == true) {
                currentOperation->SendEvent(audio::Operation::Event::HeadphonesPlugin);
            }
        }
        else {
            // If creating operation failed fallback to IdleOperation which is guaranteed to work
            LOG_ERROR("Failed to create operation type %s", Operation::c_str(op));
            currentOperation = Operation::Create(Operation::Type::Idle, "").value_or(nullptr);
            currentState     = State ::Idle;
            return RetCode::OperationCreateFailed;
        }

        return currentOperation->Start(asyncCallback);
    }

    audio::RetCode Audio::Stop()
    {
        if (currentState == State::Idle) {
            return RetCode::Success;
        }

        auto retStop = currentOperation != nullptr ? currentOperation->Stop() : RetCode::OperationNotSet;
        if (retStop != RetCode::Success) {
            LOG_ERROR("Operation STOP failure: %s", audio::c_str(retStop));
        }

        auto ret = Operation::Create(Operation::Type::Idle, "");
        if (ret) {
            currentState     = State::Idle;
            currentOperation = std::move(ret.value());
            return RetCode::Success;
        }
        else {
            return RetCode::OperationCreateFailed;
        }
    }

    audio::RetCode Audio::Pause()
    {
        if (currentState == State::Idle) {
            return RetCode::InvokedInIncorrectState;
        }

        return currentOperation != nullptr ? currentOperation->Pause() : RetCode::OperationNotSet;
    }

    audio::RetCode Audio::Resume()
    {
        if (currentState == State::Idle) {
            return RetCode::InvokedInIncorrectState;
        }
        return currentOperation != nullptr ? currentOperation->Resume() : RetCode::OperationNotSet;
    }

} // namespace audio
