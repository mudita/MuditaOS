// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Audio/AudioDevice.hpp>
#include <Audio/AudioFormat.hpp>
#include <interface/profiles/A2DP/MediaContext.hpp>

namespace bluetooth
{

    class BluetoothAudioDevice : public audio::AudioDevice
    {
      public:
        BluetoothAudioDevice() = default;
        explicit BluetoothAudioDevice(MediaContext *mediaContext);
        virtual ~BluetoothAudioDevice();

        RetCode setOutputVolume(float vol) override;
        RetCode setInputGain(float gain) override;
        void setMediaContext(MediaContext *MediaContext);
        auto getTraits() const -> Traits override;
        auto getSupportedFormats() -> std::vector<audio::AudioFormat> override;
        auto getSourceFormat() -> audio::AudioFormat override;

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
        float outputVolume = 0.0;
    };

} // namespace bluetooth
