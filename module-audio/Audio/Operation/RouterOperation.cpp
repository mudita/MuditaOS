/*
 *  @file RouterOperation.cpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 25.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#include "RouterOperation.hpp"

#include "../Profiles/ProfileRoutingEarspeaker.hpp"
#include "../Profiles/ProfileRoutingHeadset.hpp"
#include "../Profiles/ProfileRoutingSpeakerphone.hpp"

#include "log/log.hpp"

namespace audio {

    RouterOperation::RouterOperation([[maybe_unused]]const char *file) {

        audioDeviceCallback = [this](const void *inputBuffer,
                                     void *outputBuffer,
                                     unsigned long framesPerBuffer) -> int32_t {

            if (framesPerBuffer > audioDeviceBuffer.capacity()) {
                audioDeviceBuffer.resize(framesPerBuffer);
            }

            if (inputBuffer) {
                if(muteEnable){
                    memset(&audioDeviceBuffer[0], 0, framesPerBuffer * sizeof(int16_t));
                }
                else{
                    memcpy(&audioDeviceBuffer[0], inputBuffer, framesPerBuffer * sizeof(int16_t));
                }

                if (recorderWorkerHandle) {
                    //memcpy(&channel1Buffer[0],inputBuffer,framesPerBuffer * sizeof(int16_t));
                    // xTaskNotify(recorderWorkerHandle,static_cast<uint32_t>(RecorderEvent ::Channel1Ready),eSetBits);
                }

            }

            if (outputBuffer) {
                memcpy(outputBuffer, &audioDeviceCellularBuffer[0], framesPerBuffer * sizeof(int16_t));
            }
            return framesPerBuffer;

        };

        audioDeviceCellularCallback = [this](const void *inputBuffer,
                                             void *outputBuffer,
                                             unsigned long framesPerBuffer) -> int32_t {

            if (framesPerBuffer > audioDeviceCellularBuffer.capacity()) {
                audioDeviceCellularBuffer.resize(framesPerBuffer);
            }

            if (inputBuffer) {
                memcpy(&audioDeviceCellularBuffer[0], inputBuffer, framesPerBuffer * sizeof(int16_t));

                if (recorderWorkerHandle) {
                    //memcpy(&channel2Buffer[0], inputBuffer, framesPerBuffer * sizeof(int16_t));
                    //xTaskNotify(recorderWorkerHandle, static_cast<uint32_t>(RecorderEvent::Channel2Ready), eSetBits);
                }
            }

            if (outputBuffer) {
                memcpy(outputBuffer, &audioDeviceBuffer[0], framesPerBuffer * sizeof(int16_t));
            }
            return framesPerBuffer;

        };

        audioDeviceBuffer.reserve(1024);
        audioDeviceCellularBuffer.reserve(1024);

        //TODO:M.P should be fetched from DB
        availableProfiles.push_back(std::make_unique<ProfileRoutingEarspeaker>(nullptr, 1, 5));
        availableProfiles.push_back(std::make_unique<ProfileRoutingSpeakerphone>(nullptr, 1, 2));
        availableProfiles.push_back(std::make_unique<ProfileRoutingHeadset>(nullptr, 1, 5));

        currentProfile = availableProfiles[1].get(); // todo change back to 0

        audioDevice = bsp::AudioDevice::Create(currentProfile->GetAudioDeviceType(), audioDeviceCallback).value_or(
                nullptr);
        if (audioDevice == nullptr) {
            LOG_ERROR("Error creating AudioDevice");
            return;
        }

        audioDeviceCellular = bsp::AudioDevice::Create(bsp::AudioDevice::Type::Cellular,
                                                       audioDeviceCellularCallback).value_or(
                nullptr);
        if (audioDeviceCellular == nullptr) {
            LOG_ERROR("Error creating AudioDeviceCellular");
            return;
        }

        isInitialized = true;
    }

    RouterOperation::~RouterOperation() {
        StopRecording();
    }

    int32_t RouterOperation::SetOutputVolume(float vol) {
        currentProfile->SetOutputVolume(vol);
        audioDevice->OutputVolumeCtrl(vol);
        return static_cast<int32_t >(RetCode::Success);
    }

    int32_t RouterOperation::SetInputGain(float gain) {
        currentProfile->SetInputGain(gain);
        audioDevice->InputGainCtrl(gain);
        return static_cast<int32_t >(RetCode::Success);
    }

    int32_t RouterOperation::Start(std::function<int32_t(audio::AudioEvents)> callback) {
        if (state == State::Paused || state == State::Active) {
            return static_cast<int32_t >(RetCode::InvokedInIncorrectState);
        }

        eventCallback = callback;
        state = State::Active;

        int32_t ret = 0;
        if (audioDevice->IsFormatSupported(currentProfile->GetAudioFormat())) {
            ret = audioDevice->Start(currentProfile->GetAudioFormat());
        } else {
            return static_cast<int32_t >(RetCode::InvalidFormat);
        }

        if (audioDeviceCellular->IsFormatSupported(currentProfile->GetAudioFormat())) {
            return audioDeviceCellular->Start(currentProfile->GetAudioFormat());
        } else {
            return static_cast<int32_t >(RetCode::InvalidFormat);
        }
    }

    int32_t RouterOperation::Stop() {
        if (state == State::Paused || state == State::Idle) {
            return static_cast<int32_t >(RetCode::InvokedInIncorrectState);
        }

        state = State::Idle;
        audioDevice->Stop();
        audioDeviceCellular->Stop();

        StopRecording();
    }

    int32_t RouterOperation::Pause() {
        if (state == State::Paused || state == State::Idle) {
            return static_cast<int32_t >(RetCode::InvokedInIncorrectState);
        }

        state = State::Paused;
        audioDevice->Stop();
        audioDeviceCellular->Stop();
    }

    int32_t RouterOperation::Resume() {
        if (state == State::Active || state == State::Idle) {
            return static_cast<int32_t >(RetCode::InvokedInIncorrectState);
        }

        state = State::Active;
        audioDevice->Start(currentProfile->GetAudioFormat());
        audioDeviceCellular->Start(currentProfile->GetAudioFormat());
    }

    int32_t RouterOperation::SendEvent(const audio::Operation::Event evt, const audio::EventData *data) {
        switch (evt) {
            case Event::HeadphonesPlugin:
                SwitchProfile(Profile::Type::RoutingHeadset);
                break;
            case Event::HeadphonesUnplug:
                SwitchProfile(Profile::Type::RoutingEarspeaker);
                break;
            case Event::BTHeadsetOn:
                //TODO:M.P SwitchProfile(Profile::Type::RecordingBTHeadset);
                break;
            case Event::BTHeadsetOff:
                //TODO:M.P SwitchProfile(Profile::Type::RecordingBuiltInMic);
                break;
            case Event::StartCallRecording:
                //StartRecording(); TODO: M.P For now it's still in development
                break;
            case Event::StopCallRecording:
                //StopRecording(); TODO: M.P For now it's still in development
                break;
            case Event ::CallMute:
                Mute(true);
                break;
            case Event ::CallUnmute:
                Mute(false);
                break;
            case Event ::CallSpeakerphoneOn:
                SwitchProfile(Profile::Type::RoutingSpeakerphone);
                break;
            case Event ::CallSpeakerphoneOff:
                SwitchProfile(Profile::Type::RoutingEarspeaker);
                break;
            default:
                return static_cast<int32_t >(RetCode::UnsupportedProfile);

        }

        return static_cast<int32_t >(RetCode::Success);
    }

    int32_t RouterOperation::SwitchProfile(const audio::Profile::Type type) {
        auto ret = GetProfile(type);
        if (ret) {
            currentProfile = ret.value();
        } else {
            return static_cast<int32_t >(RetCode::UnsupportedProfile);
        }

        audioDevice = bsp::AudioDevice::Create(currentProfile->GetAudioDeviceType(), audioCallback).value_or(nullptr);
/*        audioDeviceCellular = bsp::AudioDevice::Create(bsp::AudioDevice::Type::Cellular, audioCallback).value_or(
                nullptr);*/

        switch (state) {
            case State::Idle:
            case State::Paused:
                break;

            case State::Active:
                state = State::Idle;
                Start(eventCallback);
                break;
        }


        //TODO:M.P add error handling
        return 0;
    }

    bool RouterOperation::Mute(bool enable) {
        muteEnable = enable;
    }

    int32_t RouterOperation::StartRecording() {
        channel1Buffer.reserve(1024);
        channel2Buffer.reserve(1024);
        mixBuffer.reserve(1024);

        uint32_t channels = 0;
        if ((currentProfile->GetInOutFlags() & static_cast<uint32_t >(bsp::AudioDevice::Flags::InputLeft)) ||
            (currentProfile->GetInOutFlags() & static_cast<uint32_t >(bsp::AudioDevice::Flags::InputRight))) {
            channels = 1;
        } else if (currentProfile->GetInOutFlags() & static_cast<uint32_t >(bsp::AudioDevice::Flags::InputStereo)) {
            channels = 2;
        }

        enc = Encoder::Create("callrec.wav",
                              Encoder::Format{.chanNr=channels, .sampleRate=currentProfile->GetSampleRate()});


        if (xTaskCreate(recorderWorker, "recworker", 512, this, 0, &recorderWorkerHandle) != pdPASS) {
            LOG_ERROR("Creating recworker failed");
        }


    }

    int32_t RouterOperation::StopRecording() {
        if (recorderWorkerHandle) {
            vTaskDelete(recorderWorkerHandle);
            enc.reset();
        }
    }

    void recorderWorker(void *pvp) {
        uint32_t ulNotificationValue = 0;
        RouterOperation *inst = reinterpret_cast<RouterOperation *>(pvp);
        static bool chan1_ready = 0;
        static bool chan2_ready = 0;

        while (1) {
            xTaskNotifyWait(0x00,               /* Don't clear any bits on entry. */
                            UINT32_MAX,          /* Clear all bits on exit. */
                            &ulNotificationValue, /* Receives the notification value. */
                            portMAX_DELAY);    /* Block indefinitely. */


            if (ulNotificationValue & static_cast<uint32_t >(RouterOperation::RecorderEvent::Channel1Ready)) {
                chan1_ready = true;
            }

            if (ulNotificationValue & static_cast<uint32_t >(RouterOperation::RecorderEvent::Channel2Ready)) {
                chan2_ready = true;
            }

            int16_t *chan1_ptr = &inst->channel1Buffer[0];
            int16_t *chan2_ptr = &inst->channel2Buffer[0];
            int16_t *mixed_ptr = &inst->mixBuffer[0];
            int32_t mixed_sample = 0;

            // Mix samples & encode them
            if (chan1_ready && chan2_ready) {

                for (uint32_t i = 0; i < inst->mixBuffer.capacity(); i++) {
                    mixed_sample = *chan1_ptr + *chan2_ptr;
                    if (mixed_sample > INT16_MAX) {
                        *mixed_ptr = INT16_MAX;
                    } else if (mixed_sample < INT16_MIN) {
                        *mixed_ptr = INT16_MIN;
                    } else {
                        *mixed_ptr = (int16_t) mixed_sample;
                    }

                    mixed_ptr++;
                    chan1_ptr++;
                    chan2_ptr++;
                }

                size_t bytesWritten = inst->enc->Encode(inst->mixBuffer.capacity(), &inst->mixBuffer[0]);
                if (bytesWritten == 0) {
                    LOG_ERROR("Encoder failed");
                }

                chan1_ready = false;
                chan2_ready = false;
            }
        }


    }

}