// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RouterOperation.hpp"

#include <Audio/AudioDevice.hpp>
#include <Audio/AudioCommon.hpp>
#include <Audio/Profiles/Profile.hpp>
#include <Audio/StreamFactory.hpp>

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

        // check if audio devices support desired audio format
        if (!audioDevice->IsFormatSupported(currentProfile->GetAudioFormat())) {
            return RetCode::InvalidFormat;
        }

        if (!audioDeviceCellular->IsFormatSupported(currentProfile->GetAudioFormat())) {
            return RetCode::InvalidFormat;
        }

        // try to run devices with the format
        if (auto ret = audioDevice->Start(currentProfile->GetAudioFormat()); ret != AudioDevice::RetCode::Success) {
            return GetDeviceError(ret);
        }

        if (auto ret = audioDeviceCellular->Start(currentProfile->GetAudioFormat());
            ret != AudioDevice::RetCode::Success) {
            return GetDeviceError(ret);
        }

        // create streams
        StreamFactory streamFactory(routerCapabilities);
        dataStreamIn  = streamFactory.makeStream(*audioDevice.get(), *audioDeviceCellular.get());
        dataStreamOut = streamFactory.makeStream(*audioDevice.get(), *audioDeviceCellular.get());

        // create audio connections
        inputConnection =
            std::make_unique<audio::StreamConnection>(audioDeviceCellular.get(), audioDevice.get(), dataStreamIn.get());
        outputConnection = std::make_unique<audio::StreamConnection>(
            audioDevice.get(), audioDeviceCellular.get(), dataStreamOut.get());

        // enable audio connections
        inputConnection->enable();
        outputConnection->enable();

        return audio::RetCode::Success;
    }

    audio::RetCode RouterOperation::Stop()
    {
        if (state == State::Paused || state == State::Idle) {
            return RetCode::InvokedInIncorrectState;
        }

        state = State::Idle;
        outputConnection.reset();
        inputConnection.reset();

        audioDevice->Stop();
        audioDeviceCellular->Stop();

        audioDevice.reset();
        audioDeviceCellular.reset();

        return RetCode::Success;
    }

    audio::RetCode RouterOperation::Pause()
    {
        if (state == State::Paused || state == State::Idle) {
            return RetCode::InvokedInIncorrectState;
        }

        state = State::Paused;
        outputConnection->disable();
        inputConnection->disable();
        return RetCode::Success;
    }

    audio::RetCode RouterOperation::Resume()
    {
        if (state == State::Active || state == State::Idle) {
            return RetCode::InvokedInIncorrectState;
        }

        state = State::Active;
        inputConnection->enable();
        outputConnection->enable();
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
            SetProfileAvailability({Profile::Type::RoutingEarspeaker}, false);
            SwitchProfile(Profile::Type::RoutingLoudspeaker);
            break;
        case EventType::CallLoudspeakerOff:
            SetProfileAvailability({Profile::Type::RoutingEarspeaker}, true);
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
        auto newProfile     = GetProfile(type);
        auto callInProgress = state == State::Active;

        if (newProfile == nullptr) {
            return RetCode::UnsupportedProfile;
        }

        if (currentProfile && currentProfile->GetType() == newProfile->GetType()) {
            return RetCode::Success;
        }

        if (callInProgress) {
            Stop();
        }

        audioDevice = CreateDevice(newProfile->GetAudioDeviceType());
        if (audioDevice == nullptr) {
            LOG_ERROR("Error creating AudioDevice");
            return RetCode::Failed;
        }

        audioDeviceCellular = CreateDevice(AudioDevice::Type::Cellular);
        if (audioDeviceCellular == nullptr) {
            LOG_ERROR("Error creating AudioDeviceCellular");
            return RetCode::Failed;
        }

        // store new profile
        currentProfile = newProfile;

        if (callInProgress) {
            return Start(operationToken);
        }

        return RetCode::Success;
    }

    bool RouterOperation::Mute(bool enable)
    {
        if (enable == true) {
            outputConnection->disable();
        }
        else {
            outputConnection->enable();
        }
        return true;
    }

    Position RouterOperation::GetPosition()
    {
        return 0.0;
    }

    RouterOperation::~RouterOperation()
    {
        Stop();
    }

} // namespace audio
