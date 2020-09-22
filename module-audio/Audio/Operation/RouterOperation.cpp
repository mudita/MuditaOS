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

namespace audio
{

    RouterOperation::RouterOperation(
        [[maybe_unused]] const char *file,
        std::function<uint32_t(const std::string &path, const uint32_t &defaultValue)> dbCallback)
    {

        audioDeviceCallback =
            [this](const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer) -> int32_t {
            if (inputBuffer != nullptr) {
                if (framesPerBuffer > audioDeviceBuffer.size()) {
                    audioDeviceBuffer.resize(framesPerBuffer, 0);
                }

                if (muteEnable) {
                    memset(&audioDeviceBuffer[0], 0, framesPerBuffer * sizeof(int16_t));
                }
                else {
                    memcpy(&audioDeviceBuffer[0], inputBuffer, framesPerBuffer * sizeof(int16_t));
                }

                if (recorderWorkerHandle) {
                    channel1Buffer = audioDeviceBuffer;
                    xTaskNotify(recorderWorkerHandle, static_cast<uint32_t>(RecorderEvent ::Channel1Ready), eSetBits);
                }
            }

            if (outputBuffer != nullptr) {
                if (framesPerBuffer > audioDeviceCellularBuffer.size()) {
                    audioDeviceCellularBuffer.resize(framesPerBuffer, 0);
                }

                memcpy(outputBuffer, &audioDeviceCellularBuffer[0], framesPerBuffer * sizeof(int16_t));
            }
            return framesPerBuffer;
        };

        audioDeviceCellularCallback =
            [this](const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer) -> int32_t {
            if (inputBuffer != nullptr) {
                if (framesPerBuffer > audioDeviceCellularBuffer.size()) {
                    audioDeviceCellularBuffer.resize(framesPerBuffer, 0);
                }

                memcpy(&audioDeviceCellularBuffer[0], inputBuffer, framesPerBuffer * sizeof(int16_t));

                if (recorderWorkerHandle) {
                    channel2Buffer = audioDeviceCellularBuffer;
                    xTaskNotify(recorderWorkerHandle, static_cast<uint32_t>(RecorderEvent::Channel2Ready), eSetBits);
                }
            }

            if (outputBuffer != nullptr) {
                if (framesPerBuffer > audioDeviceBuffer.size()) {
                    audioDeviceBuffer.resize(framesPerBuffer, 0);
                }

                memcpy(outputBuffer, &audioDeviceBuffer[0], framesPerBuffer * sizeof(int16_t));
            }
            return framesPerBuffer;
        };

        audioDeviceBuffer.resize(1024, 0);
        audioDeviceCellularBuffer.resize(1024, 0);

        constexpr audio::Gain defaultRoutingEarspeakerGain       = 20;
        constexpr audio::Volume defaultRoutingEarspeakerVolume   = 10;
        constexpr audio::Gain defaultRoutingSpeakerphoneGain     = 20;
        constexpr audio::Volume defaultRoutingSpeakerphoneVolume = 10;
        constexpr audio::Gain defaultRoutingHeadsetGain          = 50;
        constexpr audio::Volume defaultRoutingHeadsetVolume      = 10;

        const auto dbRoutingEarspeakerGainPath =
            audio::str(audio::Profile::Type::RoutingEarspeaker, audio::Setting::Gain);
        const auto routingEarspeakerGain = dbCallback(dbRoutingEarspeakerGainPath, defaultRoutingEarspeakerGain);
        const auto dbRoutingEarspeakerVolumePath =
            audio::str(audio::Profile::Type::RoutingEarspeaker, audio::Setting::Volume);
        const auto routingEarspeakerVolume = dbCallback(dbRoutingEarspeakerVolumePath, defaultRoutingEarspeakerVolume);
        const auto dbRoutingSpeakerphoneGainPath =
            audio::str(audio::Profile::Type::RoutingSpeakerphone, audio::Setting::Gain);
        const auto routingSpeakerphoneGain = dbCallback(dbRoutingSpeakerphoneGainPath, defaultRoutingSpeakerphoneGain);
        const auto dbRoutingSpeakerphoneVolumePath =
            audio::str(audio::Profile::Type::RoutingSpeakerphone, audio::Setting::Volume);
        const auto routingSpeakerphoneVolume =
            dbCallback(dbRoutingSpeakerphoneVolumePath, defaultRoutingSpeakerphoneVolume);
        const auto dbRoutingHeadsetGainPath = audio::str(audio::Profile::Type::RoutingHeadset, audio::Setting::Gain);
        const auto routingHeadsetGain = dbCallback(dbRoutingHeadsetGainPath, defaultRoutingHeadsetGain);
        const auto dbRoutingHeadsetVolumePath =
            audio::str(audio::Profile::Type::RoutingHeadset, audio::Setting::Volume);
        const auto routingHeadsetVolume = dbCallback(dbRoutingHeadsetVolumePath, defaultRoutingHeadsetVolume);

        availableProfiles.push_back(
            std::make_unique<ProfileRoutingEarspeaker>(nullptr, routingEarspeakerVolume, routingEarspeakerGain));
        availableProfiles.push_back(
            std::make_unique<ProfileRoutingSpeakerphone>(nullptr, routingSpeakerphoneVolume, routingSpeakerphoneGain));
        availableProfiles.push_back(
            std::make_unique<ProfileRoutingHeadset>(nullptr, routingHeadsetVolume, routingHeadsetGain));

        currentProfile = availableProfiles[0].get();

        audioDevice =
            bsp::AudioDevice::Create(currentProfile->GetAudioDeviceType(), audioDeviceCallback).value_or(nullptr);
        if (audioDevice == nullptr) {
            LOG_ERROR("Error creating AudioDevice");
            return;
        }

        audioDeviceCellular =
            bsp::AudioDevice::Create(bsp::AudioDevice::Type::Cellular, audioDeviceCellularCallback).value_or(nullptr);
        if (audioDeviceCellular == nullptr) {
            LOG_ERROR("Error creating AudioDeviceCellular");
            return;
        }

        isInitialized = true;
    }

    RouterOperation::~RouterOperation()
    {
        StopRecording();
    }

    audio::RetCode RouterOperation::SetOutputVolume(float vol)
    {
        currentProfile->SetOutputVolume(vol);
        auto ret = audioDevice->OutputVolumeCtrl(vol);
        return GetDeviceError(ret);
    }

    audio::RetCode RouterOperation::SetInputGain(float gain)
    {
        currentProfile->SetInputGain(gain);
        auto ret = audioDevice->InputGainCtrl(gain);
        return GetDeviceError(ret);
    }

    audio::RetCode RouterOperation::Start(std::function<int32_t(audio::AudioEvents)> callback)
    {
        if (state == State::Paused || state == State::Active) {
            return RetCode::InvokedInIncorrectState;
        }

        eventCallback = callback;
        state         = State::Active;

        if (audioDevice->IsFormatSupported(currentProfile->GetAudioFormat())) {
            auto ret = audioDevice->Start(currentProfile->GetAudioFormat());
            if (ret != bsp::AudioDevice::RetCode::Success) {
                LOG_ERROR("Start error: %s", audio::c_str(audio::RetCode::DeviceFailure));
            }
        }
        else {
            return RetCode::InvalidFormat;
        }

        if (audioDeviceCellular->IsFormatSupported(currentProfile->GetAudioFormat())) {
            auto ret = audioDeviceCellular->Start(currentProfile->GetAudioFormat());
            return GetDeviceError(ret);
        }
        else {
            return RetCode::InvalidFormat;
        }
    }

    audio::RetCode RouterOperation::Stop()
    {
        if (state == State::Paused || state == State::Idle) {
            return RetCode::InvokedInIncorrectState;
        }

        state = State::Idle;
        audioDevice->Stop();
        audioDeviceCellular->Stop();

        StopRecording();
        return RetCode::Success;
    }

    audio::RetCode RouterOperation::Pause()
    {
        if (state == State::Paused || state == State::Idle) {
            return RetCode::InvokedInIncorrectState;
        }

        state = State::Paused;
        audioDevice->Stop();
        audioDeviceCellular->Stop();
        return RetCode::Success;
    }

    audio::RetCode RouterOperation::Resume()
    {
        if (state == State::Active || state == State::Idle) {
            return RetCode::InvokedInIncorrectState;
        }

        state = State::Active;
        audioDevice->Start(currentProfile->GetAudioFormat());
        audioDeviceCellular->Start(currentProfile->GetAudioFormat());
        return RetCode::Success;
    }

    audio::RetCode RouterOperation::SendEvent(const audio::Operation::Event evt, const audio::EventData *data)
    {
        switch (evt) {
        case Event::HeadphonesPlugin:
            SwitchProfile(Profile::Type::RoutingHeadset);
            break;
        case Event::HeadphonesUnplug:
            // TODO: Switch to routing bt profile if present
            SwitchProfile(Profile::Type::RoutingEarspeaker);
            break;
        case Event::BTHeadsetOn:
            // TODO:M.P SwitchProfile(Profile::Type::RecordingBTHeadset);
            break;
        case Event::BTHeadsetOff:
            // TODO:M.P SwitchProfile(Profile::Type::RecordingBuiltInMic);
            break;
        case Event::StartCallRecording:
            // StartRecording(); TODO: M.P For now it's still in development
            break;
        case Event::StopCallRecording:
            // StopRecording(); TODO: M.P For now it's still in development
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
            // TODO: Switch to routing headphones/bt profile if present
            SwitchProfile(Profile::Type::RoutingEarspeaker);
            break;
        default:
            return RetCode::UnsupportedEvent;
        }

        return RetCode::Success;
    }

    audio::RetCode RouterOperation::SwitchProfile(const audio::Profile::Type type)
    {
        auto ret = GetProfile(type);
        if (ret) {
            currentProfile = ret.value();
        }
        else {
            return RetCode::UnsupportedProfile;
        }

        audioDevice =
            bsp::AudioDevice::Create(currentProfile->GetAudioDeviceType(), audioDeviceCallback).value_or(nullptr);
        audioDeviceCellular =
            bsp::AudioDevice::Create(bsp::AudioDevice::Type::Cellular, audioDeviceCellularCallback).value_or(nullptr);

        switch (state) {
        case State::Idle:
        case State::Paused:
            break;

        case State::Active:
            state = State::Idle;
            Start(eventCallback);
            break;
        }

        return RetCode::Success;
    }

    bool RouterOperation::Mute(bool enable)
    {
        muteEnable = enable;
        return true;
    }

    audio::RetCode RouterOperation::StartRecording()
    {
        channel1Buffer.reserve(1024);
        channel2Buffer.reserve(1024);
        mixBuffer.reserve(1024);

        uint32_t channels = 0;
        if ((currentProfile->GetInOutFlags() & static_cast<uint32_t>(bsp::AudioDevice::Flags::InputLeft)) ||
            (currentProfile->GetInOutFlags() & static_cast<uint32_t>(bsp::AudioDevice::Flags::InputRight))) {
            channels = 1;
        }
        else if (currentProfile->GetInOutFlags() & static_cast<uint32_t>(bsp::AudioDevice::Flags::InputStereo)) {
            channels = 2;
        }

        // TODO:M.P create record file's name dynamically based on current date/time
        enc = Encoder::Create("sys/callrec.wav",
                              Encoder::Format{.chanNr = channels, .sampleRate = currentProfile->GetSampleRate()});

        if (enc == nullptr) {
            return RetCode::FileDoesntExist;
        }

        if (xTaskCreate(recorderWorker, "recworker", 512, this, 0, &recorderWorkerHandle) != pdPASS) {
            LOG_ERROR("Creating recworker failed");
            return RetCode::FailedToAllocateMemory;
        }

        return RetCode::Success;
    }

    audio::RetCode RouterOperation::StopRecording()
    {
        cpp_freertos::LockGuard lock(mutex);
        if (recorderWorkerHandle) {
            vTaskDelete(recorderWorkerHandle);
            enc.reset();
        }
        return RetCode::Success;
    }

    void recorderWorker(void *pvp)
    {
        uint32_t ulNotificationValue = 0;
        RouterOperation *inst        = reinterpret_cast<RouterOperation *>(pvp);
        static bool chan1_ready      = 0;
        static bool chan2_ready      = 0;

        while (1) {
            xTaskNotifyWait(0x00,                 /* Don't clear any bits on entry. */
                            UINT32_MAX,           /* Clear all bits on exit. */
                            &ulNotificationValue, /* Receives the notification value. */
                            portMAX_DELAY);       /* Block indefinitely. */
            {
                cpp_freertos::LockGuard lock(inst->mutex);

                if (ulNotificationValue & static_cast<uint32_t>(RouterOperation::RecorderEvent::Channel1Ready)) {
                    chan1_ready = true;
                }

                if (ulNotificationValue & static_cast<uint32_t>(RouterOperation::RecorderEvent::Channel2Ready)) {
                    chan2_ready = true;
                }

                int16_t *chan1_ptr   = &inst->channel1Buffer[0];
                int16_t *chan2_ptr   = &inst->channel2Buffer[0];
                int16_t *mixed_ptr   = &inst->mixBuffer[0];
                int32_t mixed_sample = 0;

                inst->mixBuffer.resize(inst->channel1Buffer.size());

                // Mix samples & encode them
                if (chan1_ready && chan2_ready) {

                    for (uint32_t i = 0; i < inst->mixBuffer.size(); i++) {
                        mixed_sample = *chan1_ptr + *chan2_ptr;
                        if (mixed_sample > INT16_MAX) {
                            *mixed_ptr = INT16_MAX;
                        }
                        else if (mixed_sample < INT16_MIN) {
                            *mixed_ptr = INT16_MIN;
                        }
                        else {
                            *mixed_ptr = (int16_t)mixed_sample;
                        }

                        mixed_ptr++;
                        chan1_ptr++;
                        chan2_ptr++;
                    }

                    size_t bytesWritten = inst->enc->Encode(inst->mixBuffer.size(), &inst->mixBuffer[0]);
                    if (bytesWritten == 0) {
                        LOG_ERROR("Encoder failed");
                    }

                    chan1_ready = false;
                    chan2_ready = false;
                }
            }
        }
    }

} // namespace audio
