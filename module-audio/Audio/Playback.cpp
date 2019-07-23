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

using namespace bsp;

Playback::Playback(const char *file, const Profile *profile) : dec(nullptr), audioDevice(nullptr), profile(profile) {
    dec = decoder::Create(file);
    audioDevice = AudioDevice::Create(profile->GetAudioDeviceType(), [this](const void *inputBuffer,
                                                                            void *outputBuffer,
                                                                            unsigned long framesPerBuffer) -> int32_t {

        return dec->decode(framesPerBuffer, reinterpret_cast<int16_t *>(outputBuffer));

    }).value_or(nullptr);
}

Playback::~Playback() {

}

int32_t Playback::Play() {
    auto tags = dec->fetchTags();

    // Set audio device's parameters
    audioDevice->OutputVolumeCtrl(profile->GetOutputVolume());
    audioDevice->InputGainCtrl(profile->GetInputGain());
    audioDevice->OutputPathCtrl(profile->GetOutputPath());
    audioDevice->InputPathCtrl(profile->GetInputPath());

    state = State::Play;
    return audioDevice->Start(
            AudioDevice::AudioFormat{.sampleRate_Hz=tags->sample_rate, .bitWidth=16, .flags=static_cast<uint32_t >(AudioDevice::Flags::OutPutStereo)});
}

int32_t Playback::Stop() {
    state = State::Idle;
    return audioDevice->Stop();
}

int32_t Playback::Pause() {
    state = State::Pause;
    return audioDevice->Pause();
}

int32_t Playback::Resume() {
    state = State::Play;
    return audioDevice->Resume();
}

uint32_t Playback::GetPosition() {
    return dec->getCurrentPosition();

}

int32_t Playback::SwitchProfile(const Profile *prof) {
    profile = prof;

    audioDevice.reset();

    audioDevice = AudioDevice::Create(profile->GetAudioDeviceType(), [this](const void *inputBuffer,
                                                                            void *outputBuffer,
                                                                            unsigned long framesPerBuffer) -> int32_t {

        return dec->decode(framesPerBuffer, reinterpret_cast<int16_t *>(outputBuffer));

    }).value_or(nullptr);

    switch(state){
        case State ::Idle:
            break;

        case State ::Play:
            Play();
            break;

        case State ::Pause:
            //TODO:M.P remove this nasty hack..
            Play();
            Pause();
            break;

    }


    //TODO:M.P add error handling
    return 0;
}
