/*
 *  @file RecorderOperation.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 23.07.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include "RecorderOperation.hpp"
#include "Audio/encoder/Encoder.hpp"
#include "bsp/audio/bsp_audio.hpp"
#include "Audio/Profiles/Profile.hpp"
#include "Audio/Profiles/ProfileRecordingHeadset.hpp"
#include "Audio/Profiles/ProfileRecordingOnBoardMic.hpp"
#include "Audio/AudioCommon.hpp"

#include "log/log.hpp"
#include "FreeRTOS.h"
#include "task.h"

namespace audio
{

#define PERF_STATS_ON 0

    using namespace bsp;

    RecorderOperation::RecorderOperation(const char *file)
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
                state = State::Idle;
                eventCallback(AudioEvents::FileSystemNoSpace);
            }
            return ret;
        };

        // TODO:M.P should be fetched from DB
        availableProfiles.push_back(std::make_unique<ProfileRecordingOnBoardMic>(nullptr, 20.0));
        availableProfiles.push_back(std::make_unique<ProfileRecordingHeadset>(nullptr, 10.0));

        currentProfile = availableProfiles[0].get();

        uint32_t channels = 0;
        if ((currentProfile->GetInOutFlags() & static_cast<uint32_t>(AudioDevice::Flags::InputLeft)) ||
            (currentProfile->GetInOutFlags() & static_cast<uint32_t>(AudioDevice::Flags::InputRight))) {
            channels = 1;
        }
        else if (currentProfile->GetInOutFlags() & static_cast<uint32_t>(AudioDevice::Flags::InputStereo)) {
            channels = 2;
        }

        enc = Encoder::Create(file, Encoder::Format{.chanNr = channels, .sampleRate = currentProfile->GetSampleRate()});

        if (enc == nullptr) {
            LOG_ERROR("Error during initializing encoder");
            return;
        }

        audioDevice = AudioDevice::Create(currentProfile->GetAudioDeviceType(), audioCallback).value_or(nullptr);
        if (audioDevice == nullptr) {
            LOG_ERROR("Error creating AudioDevice");
            return;
        }

        isInitialized = true;
    }

    int32_t RecorderOperation::Start(std::function<int32_t(AudioEvents event)> callback)
    {

        if (state == State::Paused || state == State::Active) {
            return static_cast<int32_t>(RetCode::InvokedInIncorrectState);
        }

        eventCallback = callback;
        state         = State::Active;

        if (audioDevice->IsFormatSupported(currentProfile->GetAudioFormat())) {
            return audioDevice->Start(currentProfile->GetAudioFormat());
        }
        else {
            return static_cast<int32_t>(RetCode::InvalidFormat);
        }
    }

    int32_t RecorderOperation::Stop()
    {

        if (state == State::Paused || state == State::Idle) {
            return static_cast<int32_t>(RetCode::InvokedInIncorrectState);
        }

        state = State::Idle;
        return audioDevice->Stop();
    }

    int32_t RecorderOperation::Pause()
    {

        if (state == State::Paused || state == State::Idle) {
            return static_cast<int32_t>(RetCode::InvokedInIncorrectState);
        }

        state = State::Paused;
        return audioDevice->Stop();
    }

    int32_t RecorderOperation::Resume()
    {

        if (state == State::Active || state == State::Idle) {
            return static_cast<int32_t>(RetCode::InvokedInIncorrectState);
        }

        state = State::Active;
        return audioDevice->Start(currentProfile->GetAudioFormat());
    }

    int32_t RecorderOperation::SendEvent(const Operation::Event evt, const EventData *data)
    {
        switch (evt) {
        case Event::HeadphonesPlugin:
            SwitchProfile(Profile::Type::RecordingHeadset);
            break;
        case Event::HeadphonesUnplug:
            // TODO: Switch to recording bt profile if present
            SwitchProfile(Profile::Type::RecordingBuiltInMic);
            break;
        case Event::BTHeadsetOn:
            SwitchProfile(Profile::Type::RecordingBTHeadset);
            break;
        case Event::BTHeadsetOff:
            // TODO: Switch to recording headphones profile if present
            SwitchProfile(Profile::Type::RecordingBuiltInMic);
            break;
        default:
            return static_cast<int32_t>(RetCode::UnsupportedEvent);
        }

        return static_cast<int32_t>(RetCode::Success);
    }

    int32_t RecorderOperation::SwitchProfile(const Profile::Type type)
    {

        auto ret = GetProfile(type);
        if (ret) {
            currentProfile = ret.value();
        }
        else {
            return static_cast<int32_t>(RetCode::UnsupportedProfile);
        }

        audioDevice = AudioDevice::Create(currentProfile->GetAudioDeviceType(), audioCallback).value_or(nullptr);

        switch (state) {
        case State::Idle:
        case State::Paused:
            break;

        case State::Active:
            state = State::Idle;
            Start(eventCallback);
            break;
        }

        // TODO:M.P add error handling
        return 0;
    }

    int32_t RecorderOperation::SetOutputVolume(float vol)
    {
        currentProfile->SetOutputVolume(vol);
        audioDevice->OutputVolumeCtrl(vol);
        return static_cast<int32_t>(RetCode::Success);
    }

    int32_t RecorderOperation::SetInputGain(float gain)
    {
        currentProfile->SetInputGain(gain);
        audioDevice->InputGainCtrl(gain);
        return static_cast<int32_t>(RetCode::Success);
    }

    Position RecorderOperation::GetPosition()
    {
        return enc->getCurrentPosition();
    }

} // namespace audio
