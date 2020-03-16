/*
 *  @file PlaybackOperation.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 23.07.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#include "PlaybackOperation.hpp"
#include "bsp/audio/bsp_audio.hpp"
#include "Audio/decoder/decoder.hpp"
#include "Audio/Profiles/Profile.hpp"
#include "Audio/Profiles/ProfilePlaybackLoudspeaker.hpp"
#include "Audio/Profiles/ProfilePlaybackHeadphones.hpp"
#include "Audio/AudioCommon.hpp"

#include "log/log.hpp"
#include "FreeRTOS.h"
#include "task.h"

namespace audio
{

#define PERF_STATS_ON 0

    PlaybackOperation::PlaybackOperation(const char *file) : dec(nullptr)
    {

        audioCallback = [this](const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer) -> int32_t {

#if PERF_STATS_ON == 1
            auto tstamp = xTaskGetTickCount();
#endif
            auto ret = dec->decode(framesPerBuffer, reinterpret_cast<int16_t *>(outputBuffer));
#if PERF_STATS_ON == 1
            LOG_DEBUG("Dec:%dms", xTaskGetTickCount() - tstamp);
            // LOG_DEBUG("Watermark:%lu",uxTaskGetStackHighWaterMark2(NULL));  M.P: left here on purpose, it's handy
            // during sf tests on hardware
#endif
            if (ret == 0) {
                state = State::Idle;
                eventCallback(AudioEvents::EndOfFile); // TODO:M.P pass proper err code
            }
            return ret;
        };

        // TODO:M.P should be fetched from DB
        availableProfiles.push_back(std::make_unique<ProfilePlaybackLoudspeaker>(nullptr, 1.0));
        availableProfiles.push_back(std::make_unique<ProfilePlaybackHeadphones>(nullptr, 0.2));
        currentProfile = availableProfiles[0].get();

        dec = decoder::Create(file);
        if (dec == nullptr) {
            LOG_ERROR("Error during initializing decoder");
            return;
        }
        audioDevice = bsp::AudioDevice::Create(currentProfile->GetAudioDeviceType(), audioCallback).value_or(nullptr);
        if (audioDevice == nullptr) {
            LOG_ERROR("Error creating AudioDevice");
            return;
        }

        isInitialized = true;
    }

    int32_t PlaybackOperation::Start(std::function<int32_t(AudioEvents event)> callback)
    {

        if (state == State::Active || state == State::Paused) {
            return static_cast<int32_t>(RetCode::InvokedInIncorrectState);
        }

        auto tags = dec->fetchTags();

        eventCallback = callback;
        state         = State::Active;

        currentProfile->SetInOutFlags(tags->num_channel == 2
                                          ? static_cast<uint32_t>(bsp::AudioDevice::Flags::OutPutStereo)
                                          : static_cast<uint32_t>(bsp::AudioDevice::Flags::OutputMono));

        currentProfile->SetSampleRate(tags->sample_rate);

        return audioDevice->Start(currentProfile->GetAudioFormat());
    }

    int32_t PlaybackOperation::Stop()
    {
        if (state == State::Idle) {
            return static_cast<int32_t>(RetCode::InvokedInIncorrectState);
        }
        state = State::Idle;
        return audioDevice->Stop();
    }

    int32_t PlaybackOperation::Pause()
    {

        if (state == State::Paused || state == State::Idle) {
            return static_cast<int32_t>(RetCode::InvokedInIncorrectState);
        }

        state = State::Paused;
        return audioDevice->Stop();
    }

    int32_t PlaybackOperation::Resume()
    {

        if (state == State::Active || state == State::Idle) {
            return static_cast<int32_t>(RetCode::InvokedInIncorrectState);
        }
        state = State::Active;
        return audioDevice->Start(currentProfile->GetAudioFormat());
    }

    int32_t PlaybackOperation::SetOutputVolume(float vol)
    {
        currentProfile->SetOutputVolume(vol);
        audioDevice->OutputVolumeCtrl(vol);
        return static_cast<int32_t>(RetCode::Success);
    }

    int32_t PlaybackOperation::SetInputGain(float gain)
    {
        currentProfile->SetInputGain(gain);
        audioDevice->InputGainCtrl(gain);
        return static_cast<int32_t>(RetCode::Success);
    }

    Position PlaybackOperation::GetPosition()
    {
        return dec->getCurrentPosition();
    }

    int32_t PlaybackOperation::SendEvent(const Operation::Event evt, const EventData *data)
    {

        switch (evt) {
        case Event::HeadphonesPlugin:
            SwitchProfile(Profile::Type::PlaybackHeadphones);
            break;
        case Event::HeadphonesUnplug:
            // TODO: Switch to playback headphones/bt profile if present
            SwitchProfile(Profile::Type::PlaybackLoudspeaker);
            break;
        case Event::BTA2DPOn:
            break;
        case Event::BTA2DPOff:
            break;
        case Event::BTHeadsetOn:
            break;
        case Event::BTHeadsetOff:
            break;
        default:
            return static_cast<int32_t>(RetCode::UnsupportedEvent);
        }
        return static_cast<int32_t>(RetCode::Success);
    }

    int32_t PlaybackOperation::SwitchProfile(const Profile::Type type)
    {

        uint32_t currentSampleRate = currentProfile->GetSampleRate();
        uint32_t currentInOutFlags = currentProfile->GetInOutFlags();

        auto ret = GetProfile(type);
        if (ret) {
            currentProfile = ret.value();
        }
        else {
            return static_cast<int32_t>(RetCode::UnsupportedProfile);
        }

        audioDevice = bsp::AudioDevice::Create(currentProfile->GetAudioDeviceType(), audioCallback).value_or(nullptr);

        currentProfile->SetSampleRate(currentSampleRate);
        currentProfile->SetInOutFlags(currentInOutFlags);

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

} // namespace audio
