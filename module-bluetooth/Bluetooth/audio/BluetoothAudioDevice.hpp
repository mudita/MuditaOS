// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Audio/Endpoint.hpp>
#include <Audio/AudioDevice.hpp>
#include <interface/profiles/A2DP/MediaContext.hpp>
#include <interface/profiles/AudioProfile.hpp>

namespace bluetooth
{

    class BluetoothAudioDevice : public audio::AudioDevice
    {
      public:
        explicit BluetoothAudioDevice(AudioProfile audioProfile);
        virtual ~BluetoothAudioDevice();

        virtual auto getProfileType() const -> AudioProfile;

        auto setOutputVolume(float vol) -> audio::AudioDevice::RetCode override;
        auto setInputGain(float gain) -> audio::AudioDevice::RetCode override;

        // Endpoint control methods
        void enableInput() override;
        void enableOutput() override;
        void disableInput() override;
        void disableOutput() override;

      protected:
        auto isInputEnabled() const -> bool;
        auto isOutputEnabled() const -> bool;
        auto fillSbcAudioBuffer() -> int;

      private:
        bool outputEnabled   = false;
        bool inputEnabled    = false;
        AudioProfile profile = AudioProfile::None;
        float outputVolume;
    };

    class A2DPAudioDevice : public BluetoothAudioDevice
    {
      public:
        explicit A2DPAudioDevice() : BluetoothAudioDevice(AudioProfile::A2DP)
        {}

        void onDataSend() override;
        void onDataReceive() override;
        auto getSupportedFormats() -> std::vector<audio::AudioFormat> override;
        auto getTraits() const -> Traits override;
        auto getSourceFormat() -> ::audio::AudioFormat override;
    };

    class HSPAudioDevice : public BluetoothAudioDevice
    {
      public:
        explicit HSPAudioDevice() : BluetoothAudioDevice(AudioProfile::HSP)
        {}

        void onDataSend() override;
        void onDataSend(std::uint16_t scoHandle);
        void onDataReceive() override;
        auto getSupportedFormats() -> std::vector<audio::AudioFormat> override;
        auto getTraits() const -> Traits override;
        auto getSourceFormat() -> ::audio::AudioFormat override;
        void enableInput() override;

        void receiveCVSD(audio::AbstractStream::Span receivedData);

      private:
        static constexpr auto packetHandleOffset = 0;
        static constexpr auto packetLengthOffset = 2;
        static constexpr auto packetDataOffset   = 3;

        std::unique_ptr<std::uint8_t[]> rxLeftovers;
        std::size_t leftoversSize = 0;
    };

} // namespace bluetooth
