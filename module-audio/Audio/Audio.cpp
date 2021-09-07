// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Audio.hpp"
#include "Operation/Operation.hpp"

#include <log.hpp>
#include <bsp/headset/headset.hpp>

namespace audio
{

    Audio::Audio(AudioServiceMessage::Callback callback) : currentOperation(), serviceCallback(callback)
    {

        auto ret = Operation::Create(Operation::Type::Idle, "", audio::PlaybackType::None, callback);
        if (ret) {
            currentOperation = std::move(ret);
        }
        audioSinkState.setConnected(EventType::JackState, bsp::headset::IsInserted());
    }

    Position Audio::GetPosition()
    {
        return currentOperation->GetPosition();
    }

    audio::RetCode Audio::SendEvent(std::shared_ptr<Event> evt)
    {
        audioSinkState.UpdateState(evt);
        UpdateProfiles();
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

        try {
            auto ret = Operation::Create(op, fileName, playbackType, serviceCallback);
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
            currentOperation = std::move(ret);
            UpdateProfiles(playbackType);
        }
        catch (const AudioInitException &audioException) {
            // If creating operation failed fallback to IdleOperation which is guaranteed to work
            LOG_ERROR(
                "Failed to create operation type %s, error message:\n%s", Operation::c_str(op), audioException.what());
            currentOperation = Operation::Create(Operation::Type::Idle);
            currentState     = State ::Idle;
            return audioException.getErrorCode();
        }

        return currentOperation->Start(token);
    }

    audio::RetCode Audio::Start()
    {
        currentOperation->Stop();
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

        muted    = Muted::False;
        auto ret = Operation::Create(Operation::Type::Idle);
        if (ret) {
            currentState     = State::Idle;
            currentOperation = std::move(ret);
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
        muted = Muted::True;
        return SetOutputVolume(0);
    }

    void Audio::SendUpdateEventsToCurrentOperation()
    {
        auto updateEvents = audioSinkState.getUpdateEvents();
        for (auto &event : updateEvents) {
            currentOperation->SendEvent(event);
        }
    }

    void Audio::UpdateProfiles()
    {
        SendUpdateEventsToCurrentOperation();
        currentOperation->SwitchToPriorityProfile();
    }

    void Audio::UpdateProfiles(audio::PlaybackType playbackType)
    {
        SendUpdateEventsToCurrentOperation();
        currentOperation->SwitchToPriorityProfile(playbackType);
    }

} // namespace audio
