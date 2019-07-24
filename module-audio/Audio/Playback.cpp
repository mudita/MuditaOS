/*
 *  @file Playback.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 23.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "Playback.hpp"
#include "bsp/audio/bsp_audio.hpp"
#include "decoder/decoder.hpp"
#include "Profiles/Profile.hpp"
#include "Common.hpp"

using namespace bsp;

Playback::Playback(const char *file, const Profile *profile) : Operation(profile),dec(nullptr)  {
    dec = decoder::Create(file);
    audioDevice = AudioDevice::Create(profile->GetAudioDeviceType(), [this](const void *inputBuffer,
                                                                            void *outputBuffer,
                                                                            unsigned long framesPerBuffer) -> int32_t {

        auto ret = dec->decode(framesPerBuffer, reinterpret_cast<int16_t *>(outputBuffer));
        if(ret == 0){
            state = State ::Idle;
            eventCallback(0); // TODO:M.P pass proper err code
        }
        return ret;

    }).value_or(nullptr);
}

Playback::~Playback() {

}

int32_t Playback::Start(std::function<int32_t (uint32_t)> callback) {

    if(state == State::Active || state == State::Paused){
        return static_cast<int32_t >(RetCode ::InvokedInIncorrectState);
    }

    auto tags = dec->fetchTags();

    // Set audio device's parameters
    audioDevice->OutputVolumeCtrl(profile->GetOutputVolume());
    audioDevice->InputGainCtrl(profile->GetInputGain());
    audioDevice->OutputPathCtrl(profile->GetOutputPath());
    audioDevice->InputPathCtrl(profile->GetInputPath());

    eventCallback = callback;
    state = State::Active;
    return audioDevice->Start(
            AudioDevice::AudioFormat{.sampleRate_Hz=tags->sample_rate, .bitWidth=16, .flags=static_cast<uint32_t >(AudioDevice::Flags::OutPutStereo)});
}

int32_t Playback::Stop() {
    if(state == State::Idle){
        return static_cast<int32_t >(RetCode ::InvokedInIncorrectState);
    }
    state = State::Idle;
    return audioDevice->Stop();
}

int32_t Playback::Pause() {

    if(state == State::Paused || state == State::Idle){
        return static_cast<int32_t >(RetCode ::InvokedInIncorrectState);
    }

    state = State::Paused;
    return audioDevice->Pause();
}

int32_t Playback::Resume() {

    if(state == State::Active || state == State::Idle){
        return static_cast<int32_t >(RetCode ::InvokedInIncorrectState);
    }

    state = State::Active;
    return audioDevice->Resume();
}

Operation::Position Playback::GetPosition() {
    return dec->getCurrentPosition();

}

int32_t Playback::SwitchProfile(const Profile *prof) {
    profile = prof;

    audioDevice.reset();

    audioDevice = AudioDevice::Create(profile->GetAudioDeviceType(), [this](const void *inputBuffer,
                                                                            void *outputBuffer,
                                                                            unsigned long framesPerBuffer) -> int32_t {

        auto ret = dec->decode(framesPerBuffer, reinterpret_cast<int16_t *>(outputBuffer));
        if(ret == 0){
            state = State ::Idle;
            eventCallback(0);// TODO:M.P pass proper err code
        }
        return ret;

    }).value_or(nullptr);

    switch(state){
        case State ::Idle:
            break;

        case State ::Active:
            state = State ::Idle;
            Start(eventCallback);
            break;

        case State ::Paused:
            //TODO:M.P remove this nasty hack..
            state = State ::Idle;
            Start(eventCallback);
            Pause();
            break;

    }


    //TODO:M.P add error handling
    return 0;
}
