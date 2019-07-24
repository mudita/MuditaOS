/*
 *  @file Recorder.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 23.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "Recorder.hpp"
#include "encoder/Encoder.hpp"
#include "bsp/audio/bsp_audio.hpp"
#include "Profiles/Profile.hpp"

using namespace bsp;

Recorder::Recorder(const char *file, const Profile *profile):profile(profile) {
    enc = Encoder::Create(file,Encoder::Format{.chanNr=1,.sampleRate=44100,.sampleSiz=16});
    audioDevice = AudioDevice::Create(profile->GetAudioDeviceType(), [this](const void *inputBuffer,
                                                                            void *outputBuffer,
                                                                            unsigned long framesPerBuffer) -> int32_t {

        auto ret = enc->Encode(framesPerBuffer, reinterpret_cast<int16_t *>(const_cast<void*>(inputBuffer)));
        if(ret == 0){
            state = State ::Idle;
            errorCallback(0);
        }
        return ret;

    }).value_or(nullptr);
}

Recorder::~Recorder() {

}

int32_t Recorder::Start() {
    // Set audio device's parameters
    audioDevice->OutputVolumeCtrl(profile->GetOutputVolume());
    audioDevice->InputGainCtrl(profile->GetInputGain());
    audioDevice->OutputPathCtrl(profile->GetOutputPath());
    audioDevice->InputPathCtrl(profile->GetInputPath());

    state = State::Recording;
    return audioDevice->Start(
            AudioDevice::AudioFormat{.sampleRate_Hz=enc->format.sampleRate, .bitWidth=enc->format.sampleSiz, .flags=static_cast<uint32_t >(AudioDevice::Flags::InputLeft)});
}

int32_t Recorder::Stop() {
    state = State::Idle;
    return audioDevice->Stop();
}

int32_t Recorder::Pause() {
    state = State::Pause;
    return audioDevice->Pause();
}

int32_t Recorder::Resume() {
    state = State::Recording;
    return audioDevice->Resume();
}

int32_t Recorder::SwitchProfile(const Profile *prof) {
    profile = prof;

    audioDevice.reset();

    audioDevice = AudioDevice::Create(profile->GetAudioDeviceType(), [this](const void *inputBuffer,
                                                                            void *outputBuffer,
                                                                            unsigned long framesPerBuffer) -> int32_t {

        auto ret = enc->Encode(framesPerBuffer, reinterpret_cast<int16_t *>(const_cast<void*>(inputBuffer)));
        if(ret == 0){
            state = State ::Idle;
        }
        return ret;

    }).value_or(nullptr);

    switch(state){
        case State ::Idle:
            break;

        case State ::Recording:
            Start();
            break;

        case State ::Pause:
            //TODO:M.P remove this nasty hack..
            Start();
            Pause();
            break;

    }


    //TODO:M.P add error handling
    return 0;
}
