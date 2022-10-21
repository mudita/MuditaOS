// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RouterOperation.hpp"

#include <Audio/AudioDevice.hpp>
#include <Audio/AudioCommon.hpp>
#include <Audio/Profiles/Profile.hpp>
#include <Audio/StreamFactory.hpp>
#include <Audio/transcode/TransformFactory.hpp>

#include <bsp/headset/headset.hpp>
#include <log/log.hpp>
#include <mutex.hpp>

#include <algorithm>
#include <optional>
#include <vector>

namespace audio
{

    RouterOperation::RouterOperation([[maybe_unused]] const std::string &filePath,
                                     AudioServiceMessage::Callback callback)
        : Operation(std::move(callback))
    {
        // order defines priority
        AddProfile(Profile::Type::RoutingHeadphones, PlaybackType::None, false);
        AddProfile(Profile::Type::RoutingBluetoothHFP, PlaybackType::None, false);
        AddProfile(Profile::Type::RoutingBluetoothHSP, PlaybackType::None, false);
        AddProfile(Profile::Type::RoutingEarspeaker, PlaybackType::None, true);
        AddProfile(Profile::Type::RoutingLoudspeaker, PlaybackType::None, true);
    }

    audio::RetCode RouterOperation::SetOutputVolume(float vol)
    {
        currentProfile->SetOutputVolume(vol);
        auto ret = audioDevice->setOutputVolume(vol);
        return GetDeviceError(ret);
    }

    audio::RetCode RouterOperation::SetInputGain(float gain)
    {
        currentProfile->SetInputGain(gain);
        auto ret = audioDevice->setInputGain(gain);
        return GetDeviceError(ret);
    }

    audio::RetCode RouterOperation::Start(audio::Token token)
    {
        if (state == State::Paused || state == State::Active) {
            return RetCode::InvokedInIncorrectState;
        }
        operationToken = token;
        state          = State::Active;

        if (auto ret = audioDevice->Start(); ret != AudioDevice::RetCode::Success) {
            return GetDeviceError(ret);
        }

        if (auto ret = audioDeviceCellular->Start(); ret != AudioDevice::RetCode::Success) {
            return GetDeviceError(ret);
        }

        // create streams
        StreamFactory streamFactory(callTimeConstraint);
        try {
            dataStreamIn  = streamFactory.makeStream(*audioDevice, *audioDeviceCellular);
            dataStreamOut = streamFactory.makeStream(*audioDeviceCellular, *audioDevice);
        }
        catch (const std::exception &e) {
            LOG_FATAL("Cannot create audio stream: %s", e.what());
            return audio::RetCode::Failed;
        }

        // create audio connections
        voiceInputConnection =
            std::make_unique<audio::StreamConnection>(audioDevice.get(), audioDeviceCellular.get(), dataStreamIn.get());
        voiceOutputConnection = std::make_unique<audio::StreamConnection>(
            audioDeviceCellular.get(), audioDevice.get(), dataStreamOut.get());

        // enable audio connections
        voiceOutputConnection->enable();
        if (!IsMuted()) {
            LOG_DEBUG("Voice input not muted");
            voiceInputConnection->enable();
        }

        return audio::RetCode::Success;
    }

    audio::RetCode RouterOperation::Stop()
    {
        if (state == State::Paused || state == State::Idle) {
            return RetCode::InvokedInIncorrectState;
        }

        state = State::Idle;
        voiceOutputConnection.reset();
        voiceInputConnection.reset();

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
        voiceOutputConnection->disable();
        voiceInputConnection->disable();
        return RetCode::Success;
    }

    audio::RetCode RouterOperation::Resume()
    {
        if (state == State::Active || state == State::Idle) {
            return RetCode::InvokedInIncorrectState;
        }

        state = State::Active;
        voiceInputConnection->enable();
        voiceOutputConnection->enable();
        return RetCode::Success;
    }

    audio::RetCode RouterOperation::SendEvent(std::shared_ptr<Event> evt)
    {
        auto isAvailable = evt->getDeviceState() == Event::DeviceState::Connected ? true : false;

        switch (evt->getType()) {
        case EventType::JackState:
            SetProfileAvailability({Profile::Type::RoutingHeadphones}, isAvailable);
            jackState = isAvailable ? JackState::Plugged : JackState::Unplugged;
            SwitchToPriorityProfile();
            break;
        case EventType::MicrophoneState: {
            auto headsetHasMicrophone = isAvailable;
            LOG_DEBUG("Microphone state event has been received, does the headset have a microphone? %s",
                      headsetHasMicrophone ? "Yes" : "No");
            setInputPathForHeadset(headsetHasMicrophone);
            SwitchToPriorityProfile();
        } break;
        case EventType::BlutoothHSPDeviceState:
            SetProfileAvailability({Profile::Type::RoutingBluetoothHSP}, isAvailable);
            SwitchToPriorityProfile();
            break;
        case EventType::BlutoothHFPDeviceState:
            SetProfileAvailability({Profile::Type::RoutingBluetoothHFP}, isAvailable);
            SwitchToPriorityProfile();
            break;
        case EventType::CallLoudspeakerOn:
            SetProfileAvailability({Profile::Type::RoutingEarspeaker}, false);
            SetProfileAvailability({Profile::Type::RoutingHeadphones}, false);
            SwitchProfile(Profile::Type::RoutingLoudspeaker);
            break;
        case EventType::CallLoudspeakerOff:
            SetProfileAvailability({Profile::Type::RoutingEarspeaker}, true);
            if (jackState == JackState::Plugged) {
                SetProfileAvailability({Profile::Type::RoutingHeadphones}, true);
            }
            SwitchToPriorityProfile();
            break;
        case EventType::CallMute:
            Mute();
            break;
        case EventType::CallUnmute:
            Unmute();
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

        if (callInProgress) {
            Stop();
        }

        audioDevice = CreateDevice(*newProfile);
        if (audioDevice == nullptr) {
            LOG_ERROR("Error creating AudioDevice");
            return RetCode::Failed;
        }

        audioDeviceCellular = createCellularAudioDevice();
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

    void RouterOperation::Mute()
    {
        voiceInputConnection->disable();
        mute = Mute::Enabled;
    }

    void RouterOperation::Unmute()
    {
        voiceInputConnection->enable();
        mute = Mute::Disabled;
    }

    auto RouterOperation::IsMuted() const noexcept -> bool
    {
        return mute == RouterOperation::Mute::Enabled;
    }

    Position RouterOperation::GetPosition()
    {
        return 0.0;
    }

    void RouterOperation::setInputPathForHeadset(bool headsetHasMicrophone)
    {
        const auto inputPath =
            headsetHasMicrophone ? audio::codec::InputPath::Headphones : audio::codec::InputPath::Microphone;
        for (const auto &p : supportedProfiles) {
            if (p.profile->GetType() == Profile::Type::RoutingHeadphones) {
                p.profile->SetInputPath(inputPath);
                return; // Don't check remaining profiles after finding the right one
            }
        }
    }

    RouterOperation::~RouterOperation()
    {
        Stop();
    }

} // namespace audio
