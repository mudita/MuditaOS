// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RouterOperation.hpp"

#include <Audio/AudioCommon.hpp>
#include <Audio/Profiles/Profile.hpp>

#include <bsp_audio.hpp>
#include <log/log.hpp>

#include <optional>
#include <cstring>

namespace audio
{

    RouterOperation::RouterOperation(
        [[maybe_unused]] const char *file,
        std::function<std::uint32_t(const std::string &path, const std::uint32_t &defaultValue)> dbCallback)
    {

        audioDeviceCallback =
            [this](const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer) -> std::int32_t {
            if (inputBuffer != nullptr) {
                if (framesPerBuffer > audioDeviceBuffer.size()) {
                    audioDeviceBuffer.resize(framesPerBuffer, 0);
                }

                if (muteEnable) {
                    memset(&audioDeviceBuffer[0], 0, framesPerBuffer * sizeof(std::int16_t));
                }
                else {
                    memcpy(&audioDeviceBuffer[0], inputBuffer, framesPerBuffer * sizeof(std::int16_t));
                }
            }

            if (outputBuffer != nullptr) {
                if (framesPerBuffer > audioDeviceCellularBuffer.size()) {
                    audioDeviceCellularBuffer.resize(framesPerBuffer, 0);
                }

                memcpy(outputBuffer, &audioDeviceCellularBuffer[0], framesPerBuffer * sizeof(std::int16_t));
            }
            return framesPerBuffer;
        };

        audioDeviceCellularCallback =
            [this](const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer) -> std::int32_t {
            if (inputBuffer != nullptr) {
                if (framesPerBuffer > audioDeviceCellularBuffer.size()) {
                    audioDeviceCellularBuffer.resize(framesPerBuffer, 0);
                }

                memcpy(&audioDeviceCellularBuffer[0], inputBuffer, framesPerBuffer * sizeof(std::int16_t));
            }

            if (outputBuffer != nullptr) {
                if (framesPerBuffer > audioDeviceBuffer.size()) {
                    audioDeviceBuffer.resize(framesPerBuffer, 0);
                }

                memcpy(outputBuffer, &audioDeviceBuffer[0], framesPerBuffer * sizeof(std::int16_t));
            }
            return framesPerBuffer;
        };

        audioDeviceBuffer.resize(1024, 0);
        audioDeviceCellularBuffer.resize(1024, 0);

        constexpr audio::Gain defaultRoutingEarspeakerGain       = 20;
        constexpr audio::Volume defaultRoutingEarspeakerVolume   = 10;
        constexpr audio::Gain defaultRoutingSpeakerphoneGain     = 20;
        constexpr audio::Volume defaultRoutingSpeakerphoneVolume = 10;
        constexpr audio::Gain defaultRoutingHeadsetGain          = 50;
        constexpr audio::Volume defaultRoutingHeadsetVolume      = 10;

        const auto dbRoutingEarspeakerGainPath =
            audio::dbPath(audio::Setting::Gain, audio::PlaybackType::None, audio::Profile::Type::RoutingEarspeaker);
        const auto routingEarspeakerGain = dbCallback(dbRoutingEarspeakerGainPath, defaultRoutingEarspeakerGain);
        const auto dbRoutingEarspeakerVolumePath =
            audio::dbPath(audio::Setting::Volume, audio::PlaybackType::None, audio::Profile::Type::RoutingEarspeaker);
        const auto routingEarspeakerVolume = dbCallback(dbRoutingEarspeakerVolumePath, defaultRoutingEarspeakerVolume);
        const auto dbRoutingSpeakerphoneGainPath =
            audio::dbPath(audio::Setting::Gain, audio::PlaybackType::None, audio::Profile::Type::RoutingSpeakerphone);
        const auto routingSpeakerphoneGain = dbCallback(dbRoutingSpeakerphoneGainPath, defaultRoutingSpeakerphoneGain);
        const auto dbRoutingSpeakerphoneVolumePath =
            audio::dbPath(audio::Setting::Volume, audio::PlaybackType::None, audio::Profile::Type::RoutingSpeakerphone);
        const auto routingSpeakerphoneVolume =
            dbCallback(dbRoutingSpeakerphoneVolumePath, defaultRoutingSpeakerphoneVolume);
        const auto dbRoutingHeadsetGainPath =
            audio::dbPath(audio::Setting::Gain, audio::PlaybackType::None, audio::Profile::Type::RoutingHeadset);
        const auto routingHeadsetGain = dbCallback(dbRoutingHeadsetGainPath, defaultRoutingHeadsetGain);
        const auto dbRoutingHeadsetVolumePath =
            audio::dbPath(audio::Setting::Volume, audio::PlaybackType::None, audio::Profile::Type::RoutingHeadset);
        const auto routingHeadsetVolume = dbCallback(dbRoutingHeadsetVolumePath, defaultRoutingHeadsetVolume);

        availableProfiles.push_back(
            Profile::Create(Profile::Type::RoutingEarspeaker, nullptr, routingEarspeakerVolume, routingEarspeakerGain));
        availableProfiles.push_back(Profile::Create(
            Profile::Type::RoutingSpeakerphone, nullptr, routingSpeakerphoneVolume, routingSpeakerphoneGain));
        availableProfiles.push_back(
            Profile::Create(Profile::Type::RoutingHeadset, nullptr, routingHeadsetVolume, routingHeadsetGain));

        currentProfile = availableProfiles[0];

        audioDevice =
            bsp::AudioDevice::Create(currentProfile->GetAudioDeviceType(), audioDeviceCallback).value_or(nullptr);
        if (audioDevice == nullptr) {
            LOG_ERROR("Error creating AudioDevice");
            return;
        }

        audioDeviceCellular =
            bsp::AudioDevice::Create(bsp::AudioDevice::Type::Cellular, audioDeviceCellularCallback).value_or(nullptr);
        if (audioDeviceCellular == nullptr) {
            LOG_ERROR("Error creating AudioDeviceCellular");
            return;
        }

        isInitialized = true;
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
        switch (evt->getType()) {
        case EventType::HeadphonesPlugin:
            SwitchProfile(Profile::Type::RoutingHeadset);
            break;
        case EventType::HeadphonesUnplug:
            SwitchProfile(Profile::Type::RoutingEarspeaker);
            break;
        case EventType::CallMute:
            Mute(true);
            break;
        case EventType::CallUnmute:
            Mute(false);
            break;
        case EventType::CallSpeakerphoneOn:
            SwitchProfile(Profile::Type::RoutingSpeakerphone);
            break;
        case EventType::CallSpeakerphoneOff:
            SwitchProfile(Profile::Type::RoutingEarspeaker);
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
            currentProfile = ret.value();
        }
        else {
            return RetCode::UnsupportedProfile;
        }

        audioDevice =
            bsp::AudioDevice::Create(currentProfile->GetAudioDeviceType(), audioDeviceCallback).value_or(nullptr);
        audioDeviceCellular =
            bsp::AudioDevice::Create(bsp::AudioDevice::Type::Cellular, audioDeviceCellularCallback).value_or(nullptr);

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

    void RouterOperation::SetData(std::shared_ptr<void> data)
    {
        // pass to AudioDevice
    }

} // namespace audio
