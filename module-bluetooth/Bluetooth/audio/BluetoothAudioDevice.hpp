// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Audio/Endpoint.hpp>
#include <Audio/AudioDevice.hpp>
#include <Audio/AudioFormat.hpp>
#include <interface/profiles/A2DP/MediaContext.hpp>
#include <interface/profiles/AudioProfile.hpp>

extern "C"
{
#include "classic/btstack_cvsd_plc.h"
}

namespace bluetooth
{

    class BluetoothAudioDevice : public audio::AudioDevice
    {
      public:
        explicit BluetoothAudioDevice(AudioProfile audioProfile);
        virtual ~BluetoothAudioDevice();

        virtual auto getProfileType() const -> AudioProfile;

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
        float outputVolume;

      private:
        bool outputEnabled   = false;
        bool inputEnabled    = false;
        AudioProfile profile = AudioProfile::None;
    };

    class A2DPAudioDevice : public BluetoothAudioDevice
    {
      public:
        explicit A2DPAudioDevice() : BluetoothAudioDevice(AudioProfile::A2DP)
        {}

        auto setOutputVolume(float vol) -> audio::AudioDevice::RetCode override;
        void onDataSend() override;
        void onDataReceive() override;
        auto getSupportedFormats() -> std::vector<audio::AudioFormat> override;
        auto getTraits() const -> Traits override;
        auto getSourceFormat() -> ::audio::AudioFormat override;

        audio::AudioDevice::RetCode Pause() override;
        audio::AudioDevice::RetCode Resume() override;
    };

    class CVSDAudioDevice : public BluetoothAudioDevice
    {
      public:
        explicit CVSDAudioDevice(const AudioProfile &profile) : BluetoothAudioDevice(profile)
        {
            btstack_cvsd_plc_init(&cvsdPlcState);
        }

        auto setOutputVolume(float vol) -> audio::AudioDevice::RetCode override;
        void onDataSend() override;
        void onDataSend(std::uint16_t scoHandle);
        void onDataReceive() override;
        auto getSupportedFormats() -> std::vector<audio::AudioFormat> override;
        auto getTraits() const -> Traits override;
        auto getSourceFormat() -> ::audio::AudioFormat override;
        void enableInput() override;

        void receiveCVSD(audio::AbstractStream::Span receivedData);

      private:
        static constexpr std::size_t scratchBufferSize = 128;

        static constexpr auto packetHandleOffset = 0;
        static constexpr auto packetStatusOffset = 1;
        static constexpr auto packetLengthOffset = 2;
        static constexpr auto packetDataOffset   = 3;

        constexpr static auto supportedBitWidth = 16U;
        constexpr static auto supportedChannels = 1;

        constexpr static auto allGoodMask = 0x30;

        auto decodeCVSD(audio::AbstractStream::Span dataToDecode) -> audio::AbstractStream::Span;

        std::unique_ptr<std::uint8_t[]> rxLeftovers;
        std::unique_ptr<std::int16_t[]> decoderBuffer;
        std::size_t leftoversSize = 0;
        btstack_cvsd_plc_state_t cvsdPlcState;
    };

} // namespace bluetooth
