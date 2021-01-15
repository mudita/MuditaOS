// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AudioCommon.hpp"
#include "Stream.hpp"

#include "bsp/audio/bsp_audio.hpp"

namespace bsp
{
    class BluetoothProxyAudio final : public AudioDevice
    {
      public:
        BluetoothProxyAudio(AudioServiceMessage::Callback callback,
                            audio::Stream &dataStreamOut,
                            audio::Stream &dataStreamIn,
                            AudioDevice::Format &format);

        ~BluetoothProxyAudio() final;

        AudioDevice::RetCode Start(const Format &format) final;
        AudioDevice::RetCode Stop() final;
        AudioDevice::RetCode OutputVolumeCtrl(float vol) final;
        AudioDevice::RetCode InputGainCtrl(float gain) final;
        AudioDevice::RetCode OutputPathCtrl(OutputPath outputPath) final;
        AudioDevice::RetCode InputPathCtrl(InputPath inputPath) final;
        bool IsFormatSupported(const Format &format) final;

        void onDataReceive() final;
        void onDataSend() final;
        void enableInput() final;
        void enableOutput() final;
        void disableInput() final;
        void disableOutput() final;

      private:
        audio::Stream &dataStreamOut;
        audio::Stream &dataStreamIn;
        AudioServiceMessage::Callback serviceCallback;
        AudioDevice::Format audioFormat;
    };

} // namespace bsp
