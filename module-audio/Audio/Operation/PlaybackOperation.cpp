// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PlaybackOperation.hpp"

#include "Audio/decoder/Decoder.hpp"
#include "Audio/Profiles/Profile.hpp"

#include "Audio/AudioCommon.hpp"

#include <log/log.hpp>

namespace audio
{

    using namespace AudioServiceMessage;
    using namespace utils;

#define PERF_STATS_ON 0

    PlaybackOperation::PlaybackOperation(const char *file, const audio::PlaybackType &playbackType, Callback callback)
        : Operation(callback, playbackType), dec(nullptr)
    {
        // order defines priority
        AddProfile(Profile::Type::PlaybackBluetoothA2DP, playbackType, false);
        AddProfile(Profile::Type::PlaybackHeadphones, playbackType, false);
        AddProfile(Profile::Type::PlaybackLoudspeaker, playbackType, true);

        auto defaultProfile = GetProfile(Profile::Type::PlaybackLoudspeaker);
        if (!defaultProfile) {
            throw AudioInitException("Error during initializing profile", RetCode::ProfileNotSet);
        }
        currentProfile = defaultProfile;

        dec = Decoder::Create(file);
        if (dec == nullptr) {
            throw AudioInitException("Error during initializing decoder", RetCode::FileDoesntExist);
        }

        auto retCode = SwitchToPriorityProfile();
        if (retCode != RetCode::Success) {
            throw AudioInitException("Failed to switch audio profile", retCode);
        }

        endOfFileCallback = [this]() {
            state          = State::Idle;
            const auto req = AudioServiceMessage::EndOfFile(operationToken);
            serviceCallback(&req);
            return std::string();
        };
    }

    audio::RetCode PlaybackOperation::Start(audio::Token token)
    {
        if (state == State::Active || state == State::Paused) {
            return RetCode::InvokedInIncorrectState;
        }
        operationToken = token;

        assert(dataStreamOut != nullptr);

        dec->startDecodingWorker(*dataStreamOut, endOfFileCallback);

        if (!tags) {
            tags = dec->fetchTags();
        }

        state         = State::Active;

        if (tags->num_channel == channel::stereoSound) {
            currentProfile->SetInOutFlags(static_cast<uint32_t>(bsp::AudioDevice::Flags::OutputStereo));
        }
        else {
            currentProfile->SetInOutFlags(static_cast<uint32_t>(bsp::AudioDevice::Flags::OutputMono));
            if (currentProfile->GetOutputPath() == bsp::AudioDevice::OutputPath::Headphones) {
                currentProfile->SetOutputPath(bsp::AudioDevice::OutputPath::HeadphonesMono);
            }
        }

        currentProfile->SetSampleRate(tags->sample_rate);

        auto ret = audioDevice->Start(currentProfile->GetAudioFormat());
        return GetDeviceError(ret);
    }

    audio::RetCode PlaybackOperation::Stop()
    {
        state = State::Idle;
        if (!audioDevice) {
            return audio::RetCode::DeviceFailure;
        }

        dec->stopDecodingWorker();
        return GetDeviceError(audioDevice->Stop());
    }

    audio::RetCode PlaybackOperation::Pause()
    {
        if (state == State::Paused || state == State::Idle) {
            return RetCode::InvokedInIncorrectState;
        }
        state = State::Paused;

        dec->stopDecodingWorker();
        return GetDeviceError(audioDevice->Stop());
    }

    audio::RetCode PlaybackOperation::Resume()
    {

        if (state == State::Active || state == State::Idle) {
            return RetCode::InvokedInIncorrectState;
        }
        state    = State::Active;

        assert(dataStreamOut != nullptr);
        dec->startDecodingWorker(*dataStreamOut, endOfFileCallback);
        auto ret = audioDevice->Start(currentProfile->GetAudioFormat());
        return GetDeviceError(ret);
    }

    audio::RetCode PlaybackOperation::SetOutputVolume(float vol)
    {
        currentProfile->SetOutputVolume(vol);
        auto ret = audioDevice->OutputVolumeCtrl(vol);
        return GetDeviceError(ret);
    }

    audio::RetCode PlaybackOperation::SetInputGain(float gain)
    {
        currentProfile->SetInputGain(gain);
        auto ret = audioDevice->InputGainCtrl(gain);
        return GetDeviceError(ret);
    }

    Position PlaybackOperation::GetPosition()
    {
        return dec->getCurrentPosition();
    }

    audio::RetCode PlaybackOperation::SendEvent(std::shared_ptr<Event> evt)
    {
        auto isAvailable = evt->getDeviceState() == Event::DeviceState::Connected ? true : false;
        switch (evt->getType()) {
        case EventType::JackState:
            SetProfileAvailability({Profile::Type::PlaybackHeadphones}, isAvailable);
            SwitchToPriorityProfile();
            break;
        case EventType::BlutoothA2DPDeviceState:
            SetProfileAvailability({Profile::Type::PlaybackBluetoothA2DP}, isAvailable);
            SwitchToPriorityProfile();
            break;
        default:
            return RetCode::UnsupportedEvent;
        }

        return RetCode::Success;
    }

    audio::RetCode PlaybackOperation::SwitchProfile(const Profile::Type type)
    {
        uint32_t currentSampleRate = currentProfile->GetSampleRate();
        uint32_t currentInOutFlags = currentProfile->GetInOutFlags();

        auto ret = GetProfile(type);
        if (ret) {
            currentProfile = ret;
        }
        else {
            return RetCode::UnsupportedProfile;
        }

        if (dec->isConnected()) {
            dec->disconnectStream();
        }

        audioDevice = CreateDevice(currentProfile->GetAudioDeviceType(), audioCallback).value_or(nullptr);
        if (audioDevice == nullptr) {
            LOG_ERROR("Error creating AudioDevice");
            return RetCode::Failed;
        }

        dec->connect(audioDevice->sink, *dataStreamOut);

        currentProfile->SetSampleRate(currentSampleRate);
        currentProfile->SetInOutFlags(currentInOutFlags);

        switch (state) {
        case State::Idle:
        case State::Paused:
            break;

        case State::Active:
            state = State::Idle;
            Start(operationToken);
            break;
        }

        return audio::RetCode::Success;
    }

    PlaybackOperation::~PlaybackOperation()
    {
        dec->stopDecodingWorker();
        Stop();
        dataStreamOut->reset();
        dataStreamIn->reset();
    }

} // namespace audio
