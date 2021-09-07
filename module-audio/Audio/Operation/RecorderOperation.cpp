// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RecorderOperation.hpp"

#include "Audio/AudioDevice.hpp"
#include "Audio/encoder/Encoder.hpp"
#include "Audio/Profiles/Profile.hpp"
#include "Audio/Profiles/ProfileRecordingHeadphones.hpp"
#include "Audio/Profiles/ProfileRecordingOnBoardMic.hpp"
#include "Audio/AudioCommon.hpp"

#include <log.hpp>
#include "FreeRTOS.h"
#include "task.h"

namespace audio
{
    using namespace AudioServiceMessage;
    using namespace utils;

#define PERF_STATS_ON 0

    RecorderOperation::RecorderOperation(const char *file, AudioServiceMessage::Callback callback) : Operation(callback)
    {

        audioCallback = [this](const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer) -> int32_t {

#if PERF_STATS_ON == 1
            auto tstamp = xTaskGetTickCount();
#endif
            auto ret = enc->Encode(framesPerBuffer, reinterpret_cast<int16_t *>(const_cast<void *>(inputBuffer)));
#if PERF_STATS_ON == 1
            LOG_DEBUG("Enc:%dms", xTaskGetTickCount() - tstamp);
            // LOG_DEBUG("Watermark:%lu",uxTaskGetStackHighWaterMark2(NULL));  M.P: left here on purpose, it's handy
            // during sf tests on hardware
#endif
            if (ret == 0) {
                state          = State::Idle;
                const auto req = AudioServiceMessage::FileSystemNoSpace(operationToken);
                serviceCallback(&req);
            }
            return ret;
        };

        // order defines priority
        AddProfile(Profile::Type::RecordingHeadphones, PlaybackType::None, false);
        AddProfile(Profile::Type::RecordingBluetoothHSP, PlaybackType::None, false);
        AddProfile(Profile::Type::RecordingBuiltInMic, PlaybackType::None, true);

        auto defaultProfile = GetProfile(Profile::Type::PlaybackLoudspeaker);
        if (!defaultProfile) {
            throw AudioInitException("Error during initializing profile", RetCode::ProfileNotSet);
        }
        currentProfile = defaultProfile;

        uint32_t channels = 0;
        if ((currentProfile->GetInOutFlags() & static_cast<uint32_t>(audio::codec::Flags::InputLeft)) ||
            (currentProfile->GetInOutFlags() & static_cast<uint32_t>(audio::codec::Flags::InputRight))) {
            channels = 1;
        }
        else if (currentProfile->GetInOutFlags() & static_cast<uint32_t>(audio::codec::Flags::InputStereo)) {
            channels = 2;
        }

        enc = Encoder::Create(file, Encoder::Format{.chanNr = channels, .sampleRate = currentProfile->GetSampleRate()});
        if (enc == nullptr) {
            throw AudioInitException("Error during initializing encoder", RetCode::InvalidFormat);
        }

        auto retCode = SwitchToPriorityProfile();
        if (retCode != RetCode::Success) {
            throw AudioInitException("Failed to switch audio profile", retCode);
        }
    }

    audio::RetCode RecorderOperation::Start(audio::Token token)
    {

        if (state == State::Paused || state == State::Active) {
            return RetCode::InvokedInIncorrectState;
        }
        operationToken = token;
        state          = State::Active;

        if (audioDevice->isFormatSupportedBySource(currentProfile->getAudioFormat())) {
            auto ret = audioDevice->Start();
            return GetDeviceError(ret);
        }
        else {
            return RetCode::InvalidFormat;
        }
    }

    audio::RetCode RecorderOperation::Stop()
    {

        if (state == State::Paused || state == State::Idle) {
            return RetCode::InvokedInIncorrectState;
        }

        state = State::Idle;
        return GetDeviceError(audioDevice->Stop());
    }

    audio::RetCode RecorderOperation::Pause()
    {

        if (state == State::Paused || state == State::Idle) {
            return RetCode::InvokedInIncorrectState;
        }

        state = State::Paused;
        return GetDeviceError(audioDevice->Stop());
    }

    audio::RetCode RecorderOperation::Resume()
    {

        if (state == State::Active || state == State::Idle) {
            return RetCode::InvokedInIncorrectState;
        }

        state    = State::Active;
        auto ret = audioDevice->Start();
        return GetDeviceError(ret);
    }

    audio::RetCode RecorderOperation::SendEvent(std::shared_ptr<Event> evt)
    {
        auto isAvailable = evt->getDeviceState() == Event::DeviceState::Connected ? true : false;
        switch (evt->getType()) {
        case EventType::JackState:
            SetProfileAvailability({Profile::Type::RecordingHeadphones}, isAvailable);
            SwitchToPriorityProfile();
            break;
        case EventType::BlutoothHSPDeviceState:
            SetProfileAvailability({Profile::Type::RecordingBluetoothHSP}, isAvailable);
            SwitchToPriorityProfile();
            break;
        default:
            return RetCode::UnsupportedEvent;
        }

        return RetCode::Success;
    }

    audio::RetCode RecorderOperation::SwitchProfile(const Profile::Type type)
    {

        auto ret = GetProfile(type);
        if (ret) {
            currentProfile = ret;
        }
        else {
            return RetCode::UnsupportedProfile;
        }

        audioDevice = CreateDevice(*currentProfile);
        if (audioDevice == nullptr) {
            LOG_ERROR("Error creating AudioDevice");
            return RetCode::Failed;
        }

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

    audio::RetCode RecorderOperation::SetOutputVolume(float vol)
    {
        currentProfile->SetOutputVolume(vol);
        auto ret = audioDevice->setOutputVolume(vol);
        return GetDeviceError(ret);
    }

    audio::RetCode RecorderOperation::SetInputGain(float gain)
    {
        currentProfile->SetInputGain(gain);
        auto ret = audioDevice->setInputGain(gain);
        return GetDeviceError(ret);
    }

    Position RecorderOperation::GetPosition()
    {
        return enc->getCurrentPosition();
    }
} // namespace audio
