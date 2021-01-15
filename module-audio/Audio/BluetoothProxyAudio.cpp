// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothProxyAudio.hpp"
#include <service-bluetooth/BluetoothMessage.hpp>

namespace bsp
{
    BluetoothProxyAudio::BluetoothProxyAudio(AudioServiceMessage::Callback callback,
                                             audio::Stream &dataStreamOut,
                                             audio::Stream &dataStreamIn,
                                             AudioDevice::Format &format)
        : AudioDevice(nullptr), dataStreamOut(dataStreamOut), dataStreamIn(dataStreamIn), serviceCallback(callback),
          audioFormat(format)
    {
        LOG_DEBUG("BluetoothProxyAudio created.");
    }

    AudioDevice::RetCode BluetoothProxyAudio::Start(const AudioDevice::Format &format)
    {
        auto msg = BluetoothProxyStartMessage(dataStreamOut, dataStreamIn, format);
        serviceCallback(&msg);
        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode BluetoothProxyAudio::Stop()
    {
        auto msg = BluetoothProxyStopMessage(audioFormat);
        serviceCallback(&msg);
        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode BluetoothProxyAudio::OutputVolumeCtrl(float vol)
    {
        audioFormat.outputVolume = vol;
        auto msg                 = BluetoothProxySetVolumeMessage(audioFormat);
        serviceCallback(&msg);
        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode BluetoothProxyAudio::InputGainCtrl(float gain)
    {
        audioFormat.inputGain = gain;
        auto msg              = BluetoothProxySetGainMessage(audioFormat);
        serviceCallback(&msg);
        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode BluetoothProxyAudio::OutputPathCtrl(AudioDevice::OutputPath outputPath)
    {
        audioFormat.outputPath = outputPath;
        auto msg               = BluetoothProxySetOutputPathMessage(audioFormat);
        serviceCallback(&msg);
        return AudioDevice::RetCode::Success;
    }

    AudioDevice::RetCode BluetoothProxyAudio::InputPathCtrl(AudioDevice::InputPath inputPath)
    {
        audioFormat.inputPath = inputPath;
        auto msg              = BluetoothProxySetInputPathMessage(audioFormat);
        serviceCallback(&msg);
        return AudioDevice::RetCode::Success;
    }

    bool BluetoothProxyAudio::IsFormatSupported(const AudioDevice::Format &format)
    {
        LOG_DEBUG("Format assumed to be supported");
        return true;
    }

    BluetoothProxyAudio::~BluetoothProxyAudio()
    {
        Stop();
    }

    void BluetoothProxyAudio::onDataReceive()
    {}

    void BluetoothProxyAudio::onDataSend()
    {}

    void BluetoothProxyAudio::enableInput()
    {}

    void BluetoothProxyAudio::enableOutput()
    {}

    void BluetoothProxyAudio::disableInput()
    {}

    void BluetoothProxyAudio::disableOutput()
    {}
} // namespace bsp
