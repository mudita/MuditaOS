#include "PlaybackOperation.hpp"

#include "Audio/decoder/decoder.hpp"
#include "Audio/Profiles/Profile.hpp"
#include "Audio/Profiles/ProfilePlaybackLoudspeaker.hpp"
#include "Audio/Profiles/ProfilePlaybackHeadphones.hpp"
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
                eventCallback(AudioEvents::EndOfFile); // TODO:M.P pass proper err code
            }
            return ret;
        };

        constexpr audio::Volume defaultLoudspeakerVolume = 10;
        constexpr audio::Volume defaultHeadphonesVolume  = 2;

        const auto dbLoudspeakerVolumePath =
            audio::str(audio::Profile::Type::PlaybackLoudspeaker, audio::Setting::Volume, playbackType);
        const auto loudspeakerVolume = dbCallback(dbLoudspeakerVolumePath, defaultLoudspeakerVolume);

        const auto dbHeadphonesVolumePath =
            audio::str(audio::Profile::Type::PlaybackHeadphones, audio::Setting::Volume, playbackType);
        const auto headphonesVolume = dbCallback(dbHeadphonesVolumePath, defaultHeadphonesVolume);

        availableProfiles.push_back(std::make_unique<ProfilePlaybackLoudspeaker>(nullptr, loudspeakerVolume));
        availableProfiles.push_back(std::make_unique<ProfilePlaybackHeadphones>(nullptr, headphonesVolume));

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

    audio::RetCode PlaybackOperation::Start(std::function<int32_t(AudioEvents event)> callback)
    {

        if (state == State::Active || state == State::Paused) {
            return RetCode::InvokedInIncorrectState;
        }

        auto tags = dec->fetchTags();

        eventCallback = callback;
        state         = State::Active;

        currentProfile->SetInOutFlags(tags->num_channel == 2
                                          ? static_cast<uint32_t>(bsp::AudioDevice::Flags::OutPutStereo)
                                          : static_cast<uint32_t>(bsp::AudioDevice::Flags::OutputMono));

        currentProfile->SetSampleRate(tags->sample_rate);

        auto ret = audioDevice->Start(currentProfile->GetAudioFormat());
        return GetDeviceError(ret);
    }

    audio::RetCode PlaybackOperation::Stop()
    {
        if (state == State::Idle) {
            return RetCode::InvokedInIncorrectState;
        }
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
        state = State::Active;
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

    audio::RetCode PlaybackOperation::SendEvent(const Operation::Event evt, const EventData *data)
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
            Start(eventCallback);
            break;
        }

        return audio::RetCode::Success;
    }

} // namespace audio
