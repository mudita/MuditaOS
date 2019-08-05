/*
 *  @file RouterOperation.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 25.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_ROUTEROPERATION_HPP
#define PUREPHONE_ROUTEROPERATION_HPP


#include <memory>
#include <optional>
#include <functional>

#include "Operation.hpp"
#include <Audio/encoder/Encoder.hpp>
#include "bsp/audio/bsp_audio.hpp"

#include "mutex.hpp"
#include "FreeRTOS.h"
#include "task.h"

namespace audio {

    void recorderWorker(void* pvp);

    class RouterOperation : public Operation {

        friend void recorderWorker(void* pvp);

    public:

        RouterOperation(const char *file);
        ~RouterOperation();

        int32_t Start([[maybe_unused]] std::function<int32_t(AudioEvents event)> callback) override final;

        int32_t Stop() override final;

        int32_t Pause() override final;

        int32_t Resume() override final;

        int32_t SendEvent(const Event evt, const EventData *data = nullptr) override final;

        int32_t SwitchProfile(const Profile::Type type) override final;

        int32_t SetOutputVolume(float vol) override final;

        int32_t SetInputGain(float gain) override final;

        Position GetPosition() override final { return 0.0; }

    private:

        enum class RecorderEvent
        {
            Channel1Ready = (1<<0),
            Channel2Ready = (1<<2)
        };

        std::unique_ptr<Encoder> enc;
        std::unique_ptr<bsp::AudioDevice> audioDevice;
        std::unique_ptr<bsp::AudioDevice> audioDeviceCellular;

        std::function<int32_t(const void *inputBuffer,
                              void *outputBuffer,
                              unsigned long framesPerBuffer)> audioDeviceCallback = nullptr;

        std::function<int32_t(const void *inputBuffer,
                              void *outputBuffer,
                              unsigned long framesPerBuffer)> audioDeviceCellularCallback = nullptr;

        std::vector<int16_t > audioDeviceBuffer;
        std::vector<int16_t > audioDeviceCellularBuffer;

        TaskHandle_t recorderWorkerHandle = nullptr;
        std::vector<int16_t > channel1Buffer;
        std::vector<int16_t > channel2Buffer;
        std::vector<int16_t > mixBuffer;

        int32_t StartRecording();
        int32_t StopRecording();
    };




}

#endif //PUREPHONE_ROUTEROPERATION_HPP
