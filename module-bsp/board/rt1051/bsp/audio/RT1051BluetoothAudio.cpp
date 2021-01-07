// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RT1051BluetoothAudio.hpp"

namespace bsp
{
    RT1051BluetoothAudio::RT1051BluetoothAudio(AudioDevice::audioCallback_t callback) : AudioDevice(callback)
    {
        isInitialized = true;
        LOG_INFO("Device created!");
    }
    AudioDevice::RetCode RT1051BluetoothAudio::Start(const AudioDevice::Format &format)
    {
        LOG_INFO("Start");
        Init();
        if ((format.flags & static_cast<uint32_t>(AudioDevice::Flags::OutputMono)) != 0u) {
            OutStart();
        }
        else if ((format.flags & static_cast<uint32_t>(AudioDevice::Flags::OutputStereo)) != 0u) {
            OutStart();
        }
        return AudioDevice::RetCode::Success;
    }
    AudioDevice::RetCode RT1051BluetoothAudio::Stop()
    {
        return AudioDevice::RetCode::Success;
    }
    AudioDevice::RetCode RT1051BluetoothAudio::OutputVolumeCtrl(float vol)
    {
        return AudioDevice::RetCode::Success;
    }
    AudioDevice::RetCode RT1051BluetoothAudio::InputGainCtrl(float gain)
    {
        return AudioDevice::RetCode::Success;
    }
    AudioDevice::RetCode RT1051BluetoothAudio::OutputPathCtrl(AudioDevice::OutputPath outputPath)
    {
        return AudioDevice::RetCode::Success;
    }
    AudioDevice::RetCode RT1051BluetoothAudio::InputPathCtrl(AudioDevice::InputPath inputPath)
    {
        return AudioDevice::RetCode::Success;
    }
    bool RT1051BluetoothAudio::IsFormatSupported(const AudioDevice::Format &format)
    {
        return true;
    }
    void outBluetoothAudioWorkerTask(void *pvp)
    {
        auto *inst    = static_cast<RT1051BluetoothAudio *>(pvp);
        auto dataSize = inst->metadata.samplesPerFrame;
        auto fatalError = false;

        if (inst->sourceQueue == nullptr) {
            LOG_FATAL("sourceQueue nullptr");
            fatalError = true;
        }

        while (!fatalError) {
            auto framesFetched = inst->GetAudioCallback()(nullptr, inst->audioData.data.data(), dataSize);
            if (framesFetched == 0) {
                break;
            }
            else if (size_t(framesFetched) < inst->audioData.data.size()) {
                std::fill(inst->audioData.data.begin() + framesFetched, inst->audioData.data.end(), 0);
            }

            if (inst->sourceQueue != nullptr) {
                xQueueSend(inst->sourceQueue, inst->audioData.data.data(), 2);
            }
            else {
                LOG_ERROR("Queue nullptr");
                vTaskDelay(2);
            }
        }

        inst->OutStop();
        inst->outWorkerThread = nullptr;
        vTaskDelete(nullptr);
    }

    void RT1051BluetoothAudio::OutStart()
    {
        if (xTaskCreate(outBluetoothAudioWorkerTask,
                        "outbluetoothaudio",
                        stackSize,
                        this,
                        tskIDLE_PRIORITY,
                        &outWorkerThread) != pdPASS) {
            LOG_ERROR("Error during creating  output bluetooth audio task");
        }
    }
    void RT1051BluetoothAudio::OutStop()
    {}

    void RT1051BluetoothAudio::Init()
    {}

    RT1051BluetoothAudio::~RT1051BluetoothAudio()
    {
        Stop();
    }
} // namespace bsp
