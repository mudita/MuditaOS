// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RouterOperation.hpp"

#include <Audio/AudioCommon.hpp>
#include <Audio/Profiles/Profile.hpp>

#include <bsp_audio.hpp>
#include <log/log.hpp>
#include <mutex.hpp>

#include <algorithm>
#include <optional>
#include <vector>

namespace audio
{

    RouterOperation::RouterOperation([[maybe_unused]] const char *file, AudioServiceMessage::Callback callback)
        : Operation(callback)
    {
        // order defines priority
        AddProfile(Profile::Type::RoutingBluetoothHSP, PlaybackType::None, false);
        AddProfile(Profile::Type::RoutingHeadphones, PlaybackType::None, false);
        AddProfile(Profile::Type::RoutingEarspeaker, PlaybackType::None, true);
        AddProfile(Profile::Type::RoutingLoudspeaker, PlaybackType::None, true);
    }

    audio::RetCode RouterOperation::SetOutputVolume(float vol)
    {
        currentProfile->SetOutputVolume(vol);
        auto ret = audioDevice->OutputVolumeCtrl(vol);
        return GetDeviceError(ret);
    }

    audio::RetCode RouterOperation::SetInputGain(float gain)
    {
        currentProfile->SetInputGain(gain);
        auto ret = audioDevice->InputGainCtrl(gain);
        return GetDeviceError(ret);
    }

    audio::RetCode RouterOperation::Start(audio::Token token)
    {
        if (state == State::Paused || state == State::Active) {
            return RetCode::InvokedInIncorrectState;
        }
        operationToken = token;
        state          = State::Active;

        if (audioDevice->IsFormatSupported(currentProfile->GetAudioFormat())) {
            auto ret = audioDevice->Start(currentProfile->GetAudioFormat());
            if (ret != bsp::AudioDevice::RetCode::Success) {
                LOG_ERROR("Start error: %s", audio::str(audio::RetCode::DeviceFailure).c_str());
            }
        }
        else {
            return RetCode::InvalidFormat;
        }

        if (audioDeviceCellular->IsFormatSupported(currentProfile->GetAudioFormat())) {
            auto ret = audioDeviceCellular->Start(currentProfile->GetAudioFormat());
            return GetDeviceError(ret);
        }
        else {
            return RetCode::InvalidFormat;
        }
    }

    audio::RetCode RouterOperation::Stop()
    {
        if (state == State::Paused || state == State::Idle) {
            return RetCode::InvokedInIncorrectState;
        }

        state = State::Idle;
        audioDevice->Stop();
        audioDeviceCellular->Stop();
        dataStreamOut->reset();
        dataStreamIn->reset();

        return RetCode::Success;
    }

    audio::RetCode RouterOperation::Pause()
    {
        if (state == State::Paused || state == State::Idle) {
            return RetCode::InvokedInIncorrectState;
        }

        state = State::Paused;
        audioDevice->Stop();
        audioDeviceCellular->Stop();
        return RetCode::Success;
    }

    audio::RetCode RouterOperation::Resume()
    {
        if (state == State::Active || state == State::Idle) {
            return RetCode::InvokedInIncorrectState;
        }

        state = State::Active;
        audioDevice->Start(currentProfile->GetAudioFormat());
        audioDeviceCellular->Start(currentProfile->GetAudioFormat());
        return RetCode::Success;
    }

    audio::RetCode RouterOperation::SendEvent(std::shared_ptr<Event> evt)
    {
        auto isAvailable = evt->getDeviceState() == Event::DeviceState::Connected ? true : false;

        switch (evt->getType()) {
        case EventType::JackState:
            SetProfileAvailability({Profile::Type::RoutingHeadphones}, isAvailable);
            SwitchToPriorityProfile();
            break;
        case EventType::BlutoothHSPDeviceState:
            SetProfileAvailability({Profile::Type::RoutingBluetoothHSP}, isAvailable);
            SwitchToPriorityProfile();
            break;
        case EventType::CallLoudspeakerOn:
            SwitchProfile(Profile::Type::RoutingLoudspeaker);
            break;
        case EventType::CallLoudspeakerOff:
            SwitchToPriorityProfile();
            break;
        case EventType::CallMute:
            Mute(true);
            break;
        case EventType::CallUnmute:
            Mute(false);
            break;
        default:
            return RetCode::UnsupportedEvent;
        }

        return RetCode::Success;
    }

    audio::RetCode RouterOperation::SwitchProfile(const audio::Profile::Type type)
    {
        auto ret = GetProfile(type);

        if (ret) {
            if (currentProfile && currentProfile->GetType() == ret->GetType()) {
                return RetCode::Success;
            }
            currentProfile = ret;
        }
        else {
            return RetCode::UnsupportedProfile;
        }

        audioDevice = CreateDevice(currentProfile->GetAudioDeviceType(), nullptr).value_or(nullptr);
        if (audioDevice == nullptr) {
            LOG_ERROR("Error creating AudioDevice");
            return RetCode::Failed;
        }
        audioDeviceCellular = CreateDevice(bsp::AudioDevice::Type::Cellular, nullptr).value_or(nullptr);
        if (audioDeviceCellular == nullptr) {
            LOG_ERROR("Error creating AudioDeviceCellular");
            return RetCode::Failed;
        }

        audioDevice->source.connect(audioDeviceCellular->sink, *dataStreamIn);
        audioDeviceCellular->source.connect(audioDevice->sink, *dataStreamOut);

        switch (state) {
        case State::Idle:
        case State::Paused:
            break;

        case State::Active:
            state = State::Idle;
            Start(operationToken);
            break;
        }

        return RetCode::Success;
    }

    bool RouterOperation::Mute(bool enable)
    {
        muteEnable = enable;
        return true;
    }

    Position RouterOperation::GetPosition()
    {
        return 0.0;
    }

} // namespace audio
