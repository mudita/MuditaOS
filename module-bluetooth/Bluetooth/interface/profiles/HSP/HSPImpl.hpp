// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "HSP.hpp"
#include "Error.hpp"
#include <interface/profiles/SCO/SCO.hpp>
#include <Audio/AudioCommon.hpp>

namespace bluetooth
{
    static constexpr int serviceBufferLength = 150;
    static constexpr int commandBufferLength = 150;

    class HSP::HSPImpl
    {
      public:
        static void packetHandler(uint8_t packetType, uint16_t channel, uint8_t *event, uint16_t eventSize);
        auto init() -> Error::Code;
        void start();
        void stop();
        void startRinging() const noexcept;
        void stopRinging() const noexcept;
        void initializeCall() const noexcept;
        void connect();
        void disconnect();
        void setDevice(const Devicei &dev);
        void setOwnerService(const sys::Service *service);
        auto getStreamData() -> std::shared_ptr<BluetoothStreamData>;
        void setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> audioDevice);

      private:
        static void sendAudioEvent(audio::EventType event, audio::Event::DeviceState state);
        static void processHCIEvent(uint8_t *event);
        static void processHSPEvent(uint8_t *event);
        static void establishAudioConnection();
        static std::array<uint8_t, serviceBufferLength> serviceBuffer;
        static constexpr uint8_t rfcommChannelNr = 1;
        static std::string agServiceName;
        static uint16_t scoHandle;
        static std::unique_ptr<SCO> sco;
        static std::unique_ptr<CellularInterface> cellularInterface;
        static std::unique_ptr<AudioInterface> audioInterface;
        static std::array<char, commandBufferLength> ATcommandBuffer;
        static const sys::Service *ownerService;
        static bool isConnected;
        static bool callAnswered;
        static bool isRinging;
        static std::shared_ptr<HSPAudioDevice> audioDevice;
        static Devicei device;
    };
} // namespace bluetooth
