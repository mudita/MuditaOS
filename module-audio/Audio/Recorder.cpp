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
#include "Common.hpp"

using namespace bsp;

Recorder::Recorder(const char *file, const Profile *profile,const Encoder::Format& frmt):Operation(profile),format(frmt) {

    enc = Encoder::Create(file,format);
    audioDevice = AudioDevice::Create(profile->GetAudioDeviceType(), [this](const void *inputBuffer,
                                                                            void *outputBuffer,
                                                                            unsigned long framesPerBuffer) -> int32_t {

        auto ret = enc->Encode(framesPerBuffer, reinterpret_cast<int16_t *>(const_cast<void*>(inputBuffer)));
        if(ret == 0){
            state = State ::Idle;
            eventCallback(0);//TODO:M.P pass correct err code
        }
        return ret;

    }).value_or(nullptr);
}

Recorder::~Recorder() {

}

int32_t Recorder::Start(std::function<int32_t (uint32_t)> callback) {

    if(state == State::Paused || state == State::Active){
        return static_cast<int32_t >(RetCode ::InvokedInIncorrectState);
    }

    // Set audio device's parameters
    audioDevice->OutputVolumeCtrl(profile->GetOutputVolume());
    audioDevice->InputGainCtrl(profile->GetInputGain());
    audioDevice->OutputPathCtrl(profile->GetOutputPath());
    audioDevice->InputPathCtrl(profile->GetInputPath());

    eventCallback = callback;
    state = State::Active;

    uint32_t flags = 0;
    if(format.chanNr == 2){
       flags = static_cast<uint32_t >(AudioDevice::Flags::InputLeft) | static_cast<uint32_t >(AudioDevice::Flags::InputRight);
    }
    else if(format.chanNr == 1){
        flags = static_cast<uint32_t >(AudioDevice::Flags::InputLeft);
    }

    return audioDevice->Start(
            AudioDevice::AudioFormat{.sampleRate_Hz=enc->format.sampleRate, .bitWidth=enc->format.sampleSiz, .flags=flags});
}

int32_t Recorder::Stop() {

    if(state == State::Paused || state == State::Idle){
        return static_cast<int32_t >(RetCode ::InvokedInIncorrectState);
    }

    state = State::Idle;
    return audioDevice->Stop();
}

int32_t Recorder::Pause() {

    if(state == State::Paused || state == State::Idle){
        return static_cast<int32_t >(RetCode ::InvokedInIncorrectState);
    }

    state = State::Paused;
    return audioDevice->Pause();
}

int32_t Recorder::Resume() {

    if(state == State::Active || state == State::Idle){
        return static_cast<int32_t >(RetCode ::InvokedInIncorrectState);
    }

    state = State::Active;
    return audioDevice->Resume();
}

int32_t Recorder::SwitchProfile(const Profile* prof) {
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

Operation::Position Recorder::GetPosition() {
    return enc->getCurrentPosition();
}
