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
#include "Audio/Common.hpp"

using namespace bsp;

PlaybackOperation::PlaybackOperation(const char *file) : dec(nullptr) {

    audioCallback = [this](const void *inputBuffer,
                           void *outputBuffer,
                           unsigned long framesPerBuffer) -> int32_t {

        auto ret = dec->decode(framesPerBuffer, reinterpret_cast<int16_t *>(outputBuffer));
        if (ret == 0) {
            state = State::Idle;
            eventCallback(0); // TODO:M.P pass proper err code
        }
        return ret;
    };

    //TODO:M.P should be fetched from DB
    availableProfiles.push_back(std::make_unique<ProfilePlaybackLoudspeaker>(nullptr, 1.0));
    availableProfiles.push_back(std::make_unique<ProfilePlaybackHeadphones>(nullptr, 0.2));
    profile = availableProfiles[0].get();

    dec = decoder::Create(file);
    audioDevice = AudioDevice::Create(profile->GetAudioDeviceType(), audioCallback).value_or(nullptr);


    isInitialized = true;
}

int32_t PlaybackOperation::Start(std::function<int32_t(uint32_t)> callback) {

    if (state == State::Active || state == State::Paused) {
        return static_cast<int32_t >(RetCode::InvokedInIncorrectState);
    }

    auto tags = dec->fetchTags();

    // Set audio device's parameters
    audioDevice->OutputVolumeCtrl(profile->GetOutputVolume());
    audioDevice->InputGainCtrl(profile->GetInputGain());
    audioDevice->OutputPathCtrl(profile->GetOutputPath());
    audioDevice->InputPathCtrl(profile->GetInputPath());

    eventCallback = callback;
    state = State::Active;
    currentFormat = AudioDevice::AudioFormat{.sampleRate_Hz=tags->sample_rate, .bitWidth=16, .flags=static_cast<uint32_t >(AudioDevice::Flags::OutPutStereo)};
    return audioDevice->Start(currentFormat);
}

int32_t PlaybackOperation::Stop() {
    if (state == State::Idle) {
        return static_cast<int32_t >(RetCode::InvokedInIncorrectState);
    }
    state = State::Idle;
    return audioDevice->Stop();
}

int32_t PlaybackOperation::Pause() {

    if (state == State::Paused || state == State::Idle) {
        return static_cast<int32_t >(RetCode::InvokedInIncorrectState);
    }

    state = State::Paused;
    return audioDevice->Stop();
}

int32_t PlaybackOperation::Resume() {

    if (state == State::Active || state == State::Idle) {
        return static_cast<int32_t >(RetCode::InvokedInIncorrectState);
    }

    state = State::Active;
    return audioDevice->Start(currentFormat);
}

Position PlaybackOperation::GetPosition() {
    return dec->getCurrentPosition();

}

int32_t PlaybackOperation::SendEvent(const Operation::Event evt, const EventData *data) {

    switch (evt) {
        case Event ::HeadphonesPlugin:
            SwitchProfile(Profile::Type::PlaybackHeadphones);
            break;
        case Event ::HeadphonesUnplug:
            SwitchProfile(Profile::Type::PlaybackLoudspeaker);
            break;
        case Event ::BTA2DPOn:
            break;
        case Event ::BTA2DPOff:
            break;
        case Event ::BTHeadsetOn:
            break;
        case Event ::BTHeadsetOff:
            break;

    }
    return static_cast<int32_t >(RetCode::Success);
}

int32_t PlaybackOperation::SwitchProfile(const Profile::Type type) {

    auto ret = GetProfile(type);
    if (ret) {
        profile = ret.value();
    } else {
        return static_cast<int32_t >(RetCode::UnsupportedProfile);
    }

    audioDevice = AudioDevice::Create(profile->GetAudioDeviceType(), audioCallback).value_or(nullptr);

    switch (state) {
        case State::Idle:
            break;

        case State::Active:
            state = State::Idle;
            Start(eventCallback);
            break;

        case State::Paused:
            //TODO:M.P remove this nasty hack..
            state = State::Idle;
            Start(eventCallback);
            Pause();
            break;

    }


    //TODO:M.P add error handling
    return 0;
}
