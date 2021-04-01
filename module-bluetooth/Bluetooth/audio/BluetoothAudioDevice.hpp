// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Audio/AudioDevice.hpp>
#include <interface/profiles/A2DP/MediaContext.hpp>

namespace bluetooth
{

    class BluetoothAudioDevice : public audio::AudioDevice
    {
      public:
        BluetoothAudioDevice() = default;
        explicit BluetoothAudioDevice(MediaContext *mediaContext);
        virtual ~BluetoothAudioDevice();

        void setMediaContext(MediaContext *MediaContext);

        auto Start(const Format &format) -> audio::AudioDevice::RetCode override;
        auto Stop() -> audio::AudioDevice::RetCode override;

        auto OutputVolumeCtrl(float vol) -> audio::AudioDevice::RetCode override;
        auto InputGainCtrl(float gain) -> audio::AudioDevice::RetCode override;
        auto OutputPathCtrl(OutputPath outputPath) -> audio::AudioDevice::RetCode override;
        auto InputPathCtrl(InputPath inputPath) -> audio::AudioDevice::RetCode override;
        auto IsFormatSupported(const Format &format) -> bool override;

        // Endpoint control methods
        void onDataSend() override;
        void onDataReceive() override;
        void enableInput() override;
        void enableOutput() override;
        void disableInput() override;
        void disableOutput() override;

      private:
        auto fillSbcAudioBuffer(MediaContext *context) -> int;

        MediaContext *ctx  = nullptr;
        bool outputEnabled = false;

        static constexpr Capabilities btCapabilities = {.usesDMA = false, .minBlockSize = 512U, .maxBlockSize = 512U};
    };

} // namespace bluetooth
