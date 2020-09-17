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

namespace audio
{

    void recorderWorker(void *pvp);

    class RouterOperation : public Operation
    {

        friend void recorderWorker(void *pvp);

      public:
        RouterOperation(const char *file,
                        std::function<uint32_t(const std::string &path, const uint32_t &defaultValue)> dbCallback);

        ~RouterOperation();

        audio::RetCode Start([[maybe_unused]] std::function<int32_t(AudioEvents event)> callback) override final;

        audio::RetCode Stop() override final;

        audio::RetCode Pause() override final;

        audio::RetCode Resume() override final;

        audio::RetCode SendEvent(const Event evt, const EventData *data = nullptr) override final;

        audio::RetCode SwitchProfile(const Profile::Type type) override final;

        audio::RetCode SetOutputVolume(float vol) override final;

        audio::RetCode SetInputGain(float gain) override final;

        Position GetPosition() override final
        {
            return 0.0;
        }

        cpp_freertos::MutexStandard mutex;

      private:
        enum class RecorderEvent
        {
            Channel1Ready = (1 << 0),
            Channel2Ready = (1 << 2)
        };

        std::unique_ptr<Encoder> enc;
        std::unique_ptr<bsp::AudioDevice> audioDevice;
        std::unique_ptr<bsp::AudioDevice> audioDeviceCellular;

        std::function<int32_t(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer)>
            audioDeviceCallback = nullptr;

        std::function<int32_t(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer)>
            audioDeviceCellularCallback = nullptr;

        std::vector<int16_t> audioDeviceBuffer;
        std::vector<int16_t> audioDeviceCellularBuffer;

        TaskHandle_t recorderWorkerHandle = nullptr;
        std::vector<int16_t> channel1Buffer;
        std::vector<int16_t> channel2Buffer;
        std::vector<int16_t> mixBuffer;

        bool muteEnable = false;

        bool Mute(bool enable);

        audio::RetCode StartRecording();

        audio::RetCode StopRecording();
    };

} // namespace audio

#endif // PUREPHONE_ROUTEROPERATION_HPP
