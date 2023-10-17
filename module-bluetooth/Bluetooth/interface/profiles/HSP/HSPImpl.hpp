// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "HSP.hpp"
#include "Result.hpp"
#include <interface/profiles/SCO/SCO.hpp>
#include <Audio/AudioCommon.hpp>

namespace bluetooth
{
    class HSP::HSPImpl
    {
      public:
        enum class HSPState
        {
            RfcommDisconnected,
            RfcommConnected,
            Ringing,
            Answered
        };

        static void packetHandler(std::uint8_t packetType,
                                  std::uint16_t channel,
                                  std::uint8_t *event,
                                  std::uint16_t eventSize);
        auto init() -> Result::Code;
        auto incomingCallStarted() const noexcept -> Result::Code;
        auto outgoingCallStarted(const std::string &number) const noexcept -> Result::Code;
        auto incomingCallAnswered() const noexcept -> Result::Code;
        auto outgoingCallAnswered() const noexcept -> Result::Code;
        auto callTerminated() const noexcept -> Result::Code;
        auto callMissed() const noexcept -> Result::Code;
        void connect();
        void disconnect();
        void setDevice(const Devicei &dev);
        void setOwnerService(sys::Service *service);
        void setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> audioDevice);

      private:
        static void sendAudioEvent(audio::EventType event, audio::Event::DeviceState state);
        static void processHCIEvent(std::uint8_t *event);
        static void processHSPEvent(std::uint8_t *event);

        static constexpr auto serviceBufferSize           = 150;
        static constexpr auto commandBufferSize           = 150;
        static constexpr std::uint8_t rfcommChannelNr     = 1;
        static constexpr std::uint32_t hspSdpRecordHandle = 0x10004;

        static hci_con_handle_t scoHandle;

        static std::uint8_t serviceBuffer[serviceBufferSize];

        static const char *agServiceName;

        static std::unique_ptr<SCO> sco;
        static std::shared_ptr<CVSDAudioDevice> audioDevice;

        static std::unique_ptr<CellularInterface> cellularInterface;
        static std::unique_ptr<AudioInterface> audioInterface;

        static sys::Service *ownerService;
        static Devicei device;

        static btstack_packet_callback_registration_t hciEventCallbackRegistration;
        static char ATCommandBuffer[commandBufferSize];
        static HSPState state;
    };
} // namespace bluetooth
