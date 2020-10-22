// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "FreeRTOS.h"
#include "macros.h"
#include "task.h"
#include "bsp/audio/bsp_audio.hpp"
#include <module-bluetooth/Bluetooth/Device.hpp>
#include <mutex.hpp>

namespace bsp
{
    class RT1051BluetoothAudio final : public AudioDevice
    {
        static constexpr uint16_t stackSize = 1024;

      public:
        RT1051BluetoothAudio(AudioDevice::audioCallback_t callback);
        ~RT1051BluetoothAudio() final;

        AudioDevice::RetCode Start(const Format &format) final;
        AudioDevice::RetCode Stop() final;
        AudioDevice::RetCode OutputVolumeCtrl(float vol) final;
        AudioDevice::RetCode InputGainCtrl(float gain) final;
        AudioDevice::RetCode OutputPathCtrl(OutputPath outputPath) final;
        AudioDevice::RetCode InputPathCtrl(InputPath inputPath) final;
        bool IsFormatSupported(const Format &format) final;

        TaskHandle_t outWorkerThread;
        AudioData_t audioData;
        QueueHandle_t sourceQueue;
        DeviceMetadata_t metadata;
        void OutStop();

      private:
        void Init();
        void OutStart();
    };

} // namespace bsp
