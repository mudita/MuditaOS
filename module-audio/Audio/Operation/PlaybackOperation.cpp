// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PlaybackOperation.hpp"

#include "Audio/decoder/decoder.hpp"
#include "Audio/Profiles/Profile.hpp"
#include "Audio/Profiles/ProfilePlaybackLoudspeaker.hpp"
#include "Audio/Profiles/ProfilePlaybackHeadphones.hpp"
#include "Audio/Profiles/ProfilePlaybackBluetoothA2DP.hpp"

#include "Audio/AudioCommon.hpp"

#include <bsp/audio/bsp_audio.hpp>
#include <log/log.hpp>
#include <FreeRTOS.h>
#include <task.h>

namespace audio
{

#define PERF_STATS_ON 0

    PlaybackOperation::PlaybackOperation(
        const char *file,
        const audio::PlaybackType &playbackType,
        std::function<uint32_t(const std::string &path, const uint32_t &defaultValue)> dbCallback)
        : Operation(false, playbackType), dec(nullptr)
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
                eventCallback({PlaybackEventType::EndOfFile, operationToken});
            }
            return ret;
        };

        constexpr audio::Volume defaultLoudspeakerVolume = 10;
        constexpr audio::Volume defaultHeadphonesVolume  = 2;

        const auto dbLoudspeakerVolumePath =
            audio::dbPath(audio::Setting::Volume, playbackType, audio::Profile::Type::PlaybackLoudspeaker);
        const auto loudspeakerVolume = dbCallback(dbLoudspeakerVolumePath, defaultLoudspeakerVolume);

        const auto dbHeadphonesVolumePath =
            audio::dbPath(audio::Setting::Volume, playbackType, audio::Profile::Type::PlaybackHeadphones);
        const auto headphonesVolume = dbCallback(dbHeadphonesVolumePath, defaultHeadphonesVolume);

        availableProfiles.push_back(Profile::Create(Profile::Type::PlaybackLoudspeaker, nullptr, loudspeakerVolume));
        availableProfiles.push_back(Profile::Create(Profile::Type::PlaybackHeadphones, nullptr, headphonesVolume));
        availableProfiles.push_back(Profile::Create(Profile::Type::PlaybackBTA2DP, nullptr, loudspeakerVolume));

        currentProfile = availableProfiles[0];

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

    audio::RetCode PlaybackOperation::Start(audio::AsyncCallback callback, audio::Token token)
    {
        if (state == State::Active || state == State::Paused) {
            return RetCode::InvokedInIncorrectState;
        }
        operationToken = token;

        if (!tags) {
            tags = dec->fetchTags();
        }

        eventCallback = callback;
        state         = State::Active;

        currentProfile->SetInOutFlags(tags->num_channel == 2
                                          ? static_cast<uint32_t>(bsp::AudioDevice::Flags::OutputStereo)
                                          : static_cast<uint32_t>(bsp::AudioDevice::Flags::OutputMono));

        currentProfile->SetSampleRate(tags->sample_rate);

        auto ret = audioDevice->Start(currentProfile->GetAudioFormat());
        return GetDeviceError(ret);
    }

    audio::RetCode PlaybackOperation::Stop()
    {
        state = State::Idle;
        return GetDeviceError(audioDevice->Stop());
    }

    audio::RetCode PlaybackOperation::Pause()
    {

        if (state == State::Paused || state == State::Idle) {
            return RetCode::InvokedInIncorrectState;
        }

        state = State::Paused;
        return GetDeviceError(audioDevice->Stop());
    }

    audio::RetCode PlaybackOperation::Resume()
    {

        if (state == State::Active || state == State::Idle) {
            return RetCode::InvokedInIncorrectState;
        }
        state    = State::Active;
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

        switch (evt->getType()) {
        case EventType::HeadphonesPlugin:
            SwitchProfile(Profile::Type::PlaybackHeadphones);
            break;
        case EventType::HeadphonesUnplug:
            // TODO: Switch to playback headphones/bt profile if present
            SwitchProfile(Profile::Type::PlaybackLoudspeaker);
            break;
        case EventType::BTA2DPOn:
            // TODO: Switch to playback headphones/bt profile if present
            SwitchProfile(Profile::Type::PlaybackBTA2DP);
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
            currentProfile = ret.value();
        }
        else {
            return RetCode::UnsupportedProfile;
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
            Start(eventCallback, operationToken);
            break;
        }

        return audio::RetCode::Success;
    }

    PlaybackOperation::~PlaybackOperation()
    {
        Stop();
    }

    void PlaybackOperation::SetBluetoothStreamData(std::shared_ptr<BluetoothStreamData> data)
    {
        if (auto device = dynamic_cast<bsp::RT1051BluetoothAudio *>(audioDevice.get()); device != nullptr) {
            device->sourceQueue = data->out;
            device->metadata    = data->metadata;
            LOG_INFO("Queue and metadata set!");
        }
    }

} // namespace audio
