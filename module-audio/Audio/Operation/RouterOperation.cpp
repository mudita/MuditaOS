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

    RouterOperation::RouterOperation(
        [[maybe_unused]] const char *file,
        std::function<std::uint32_t(const std::string &path, const std::uint32_t &defaultValue)> dbCallback)
    {
        constexpr audio::Gain defaultRoutingEarspeakerGain       = 20;
        constexpr audio::Volume defaultRoutingEarspeakerVolume   = 10;
        constexpr audio::Gain defaultRoutingSpeakerphoneGain     = 20;
        constexpr audio::Volume defaultRoutingSpeakerphoneVolume = 10;
        constexpr audio::Gain defaultRoutingHeadphonesGain       = 0;
        constexpr audio::Volume defaultRoutingHeadphonesVolume   = 10;

        const auto dbRoutingEarspeakerGainPath =
            audio::dbPath(audio::Setting::Gain, audio::PlaybackType::None, audio::Profile::Type::RoutingEarspeaker);
        const auto routingEarspeakerGain = dbCallback(dbRoutingEarspeakerGainPath, defaultRoutingEarspeakerGain);
        const auto dbRoutingEarspeakerVolumePath =
            audio::dbPath(audio::Setting::Volume, audio::PlaybackType::None, audio::Profile::Type::RoutingEarspeaker);
        const auto routingEarspeakerVolume = dbCallback(dbRoutingEarspeakerVolumePath, defaultRoutingEarspeakerVolume);
        const auto dbRoutingLoudspeakerGainPath =
            audio::dbPath(audio::Setting::Gain, audio::PlaybackType::None, audio::Profile::Type::RoutingLoudspeaker);
        const auto routingLoudspeakerGain = dbCallback(dbRoutingLoudspeakerGainPath, defaultRoutingSpeakerphoneGain);
        const auto dbRoutingLoudspeakerVolumePath =
            audio::dbPath(audio::Setting::Volume, audio::PlaybackType::None, audio::Profile::Type::RoutingLoudspeaker);
        const auto routingLoudspeakerVolume =
            dbCallback(dbRoutingLoudspeakerVolumePath, defaultRoutingSpeakerphoneVolume);
        const auto dbRoutingHeadsetGainPath =
            audio::dbPath(audio::Setting::Gain, audio::PlaybackType::None, audio::Profile::Type::RoutingHeadphones);
        const auto routingHeadphonesGain = dbCallback(dbRoutingHeadsetGainPath, defaultRoutingHeadphonesGain);
        const auto dbRoutingHeadphonesVolumePath =
            audio::dbPath(audio::Setting::Volume, audio::PlaybackType::None, audio::Profile::Type::RoutingHeadphones);
        const auto routingHeadphonesVolume = dbCallback(dbRoutingHeadphonesVolumePath, defaultRoutingHeadphonesVolume);

        // order in vector defines priority
        supportedProfiles.emplace_back(
            Profile::Create(
                Profile::Type::RoutingBluetoothHSP, nullptr, routingHeadphonesVolume, routingHeadphonesGain),
            false);
        supportedProfiles.emplace_back(
            Profile::Create(Profile::Type::RoutingHeadphones, nullptr, routingHeadphonesVolume, routingHeadphonesGain),
            false);
        supportedProfiles.emplace_back(
            Profile::Create(Profile::Type::RoutingEarspeaker, nullptr, routingEarspeakerVolume, routingEarspeakerGain),
            true);
        supportedProfiles.emplace_back(
            Profile::Create(
                Profile::Type::RoutingLoudspeaker, nullptr, routingLoudspeakerVolume, routingLoudspeakerGain),
            true);

        auto defaultProfile = GetProfile(Profile::Type::RoutingEarspeaker);
        if (!defaultProfile) {
            throw AudioInitException("Error during initializing profile", RetCode::ProfileNotSet);
        }
        currentProfile = defaultProfile;

        auto retCode = SwitchToPriorityProfile();
        if (retCode != RetCode::Success) {
            throw AudioInitException("Failed to switch audio profile", retCode);
        }
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

    audio::RetCode RouterOperation::Start(audio::AsyncCallback callback, audio::Token token)
    {
        if (state == State::Paused || state == State::Active) {
            return RetCode::InvokedInIncorrectState;
        }
        operationToken = token;
        eventCallback  = callback;
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
            currentProfile = ret;
        }
        else {
            return RetCode::UnsupportedProfile;
        }

        audioDevice = bsp::AudioDevice::Create(currentProfile->GetAudioDeviceType(), nullptr).value_or(nullptr);
        if (audioDevice == nullptr) {
            LOG_ERROR("Error creating AudioDevice");
            return RetCode::Failed;
        }

        audioDeviceCellular = bsp::AudioDevice::Create(bsp::AudioDevice::Type::Cellular, nullptr).value_or(nullptr);
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
            Start(eventCallback, operationToken);
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
