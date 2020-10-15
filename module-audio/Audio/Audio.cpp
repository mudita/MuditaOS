#include "Audio.hpp"
#include "Operation/Operation.hpp"
#include <log/log.hpp>
#include <Utils.hpp>

namespace audio
{

    Audio::Audio(AsyncCallback asyncCallback, DbCallback dbCallback)
        : currentOperation(), asyncCallback(asyncCallback), dbCallback(dbCallback)
    {

        auto ret = Operation::Create(Operation::Type::Idle, "", audio::PlaybackType::None, dbCallback);
        if (ret) {
            currentOperation = std::move(ret.value());
        }
    }

    Position Audio::GetPosition()
    {
        return currentOperation->GetPosition();
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

    audio::RetCode Audio::SendEvent(std::shared_ptr<Event> evt)
    {
        switch (evt->getType()) {
        case EventType::HeadphonesPlugin:
            headphonesInserted = true;
            break;
        case EventType::HeadphonesUnplug:
            headphonesInserted = false;
            break;
        default:
            break;
        }

        return currentOperation->SendEvent(std::move(evt));
    }

    audio::RetCode Audio::SetOutputVolume(Volume vol)
    {
        auto volToSet = vol;
        if (vol > maxVolume) {
            volToSet = maxVolume;
        }
        if (vol < minVolume) {
            volToSet = minVolume;
        }

        return currentOperation->SetOutputVolume(volToSet);
    }

    audio::RetCode Audio::SetInputGain(Gain gain)
    {
        auto gainToSet = gain;
        if (gain > maxGain) {
            gainToSet = maxGain;
        }
        if (gain < minGain) {
            gainToSet = minGain;
        }
        return currentOperation->SetInputGain(gainToSet);
    }

    audio::RetCode Audio::Start(Operation::Type op,
                                audio::Token token,
                                const char *fileName,
                                const audio::PlaybackType &playbackType)
    {

        auto ret = Operation::Create(op, fileName, playbackType, dbCallback);
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
                currentOperation->SendEvent(std::make_unique<Event>(EventType::HeadphonesPlugin));
            }
        }
        else {
            // If creating operation failed fallback to IdleOperation which is guaranteed to work
            LOG_ERROR("Failed to create operation type %s", Operation::c_str(op));
            currentOperation = Operation::Create(Operation::Type::Idle).value_or(nullptr);
            currentState     = State ::Idle;
            return RetCode::OperationCreateFailed;
        }

        return currentOperation->Start(asyncCallback, token);
    }

    audio::RetCode Audio::Start()
    {
        return Start(currentOperation->GetOperationType(),
                     currentOperation->GetToken(),
                     currentOperation->GetFilePath().c_str(),
                     currentOperation->GetPlaybackType());
    }

    audio::RetCode Audio::Stop()
    {
        if (currentState == State::Idle) {
            return RetCode::Success;
        }

        auto retStop = currentOperation->Stop();
        if (retStop != RetCode::Success) {
            LOG_ERROR("Operation STOP failure: %s", audio::str(retStop).c_str());
        }

        auto ret = Operation::Create(Operation::Type::Idle);
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

        return currentOperation->Pause();
    }

    audio::RetCode Audio::Resume()
    {
        if (currentState == State::Idle) {
            return RetCode::InvokedInIncorrectState;
        }
        return currentOperation->Resume();
    }

    audio::RetCode Audio::Mute()
    {
        return SetOutputVolume(0);
    }

} // namespace audio
