// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "HFP.hpp"
#include "Result.hpp"
#include <interface/profiles/SCO/SCO.hpp>
#include <Audio/AudioCommon.hpp>

namespace bluetooth
{
    class HFP::HFPImpl
    {
      public:
        static void packetHandler(std::uint8_t packetType,
                                  std::uint16_t channel,
                                  std::uint8_t *event,
                                  std::uint16_t eventSize);
        auto init() -> Result::Code;
        void deInit() noexcept;
        [[nodiscard]] auto incomingCallStarted() const noexcept -> Result::Code;
        [[nodiscard]] auto outgoingCallStarted(const std::string &number) const noexcept -> Result::Code;
        [[nodiscard]] auto incomingCallAnswered() const noexcept -> Result::Code;
        [[nodiscard]] auto outgoingCallAnswered() const noexcept -> Result::Code;
        [[nodiscard]] auto callTerminated() const noexcept -> Result::Code;
        [[nodiscard]] auto callMissed() const noexcept -> Result::Code;
        void connect();
        void disconnect();
        void setDevice(Devicei device);
        void setOwnerService(sys::Service *service);
        void setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> audioDevice);
        [[nodiscard]] auto setIncomingCallNumber(const std::string &num) const noexcept -> Result::Code;
        [[nodiscard]] auto setSignalStrength(int bars) const noexcept -> Result::Code;
        [[nodiscard]] auto setOperatorName(const std::string_view &name) const noexcept -> Result::Code;
        [[nodiscard]] auto setBatteryLevel(const BatteryLevel &level) const noexcept -> Result::Code;
        [[nodiscard]] auto setNetworkRegistrationStatus(bool registered) const noexcept -> Result::Code;
        [[nodiscard]] auto setRoamingStatus(bool enabled) const noexcept -> Result::Code;

      private:
        static void sendAudioEvent(audio::EventType event, audio::Event::DeviceState state);
        static void processHCIEvent(std::uint8_t *event);
        static void processHFPEvent(std::uint8_t *event);
        static void initCodecs();
        static void logSupportedCodecs();

        static constexpr auto serviceBufferSize           = 150;
        static constexpr std::uint8_t rfcommChannelNr     = 1;
        static constexpr std::uint32_t hfpSdpRecordHandle = 0x10004;

        static hci_con_handle_t scoHandle;
        static hci_con_handle_t aclHandle;

        static std::uint8_t serviceBuffer[serviceBufferSize];

        static const char *agServiceName;

        static std::unique_ptr<SCO> sco;
        static SCOCodec codec;
        static std::shared_ptr<CVSDAudioDevice> audioDevice;

        static std::unique_ptr<CellularInterface> cellularInterface;
        static std::unique_ptr<AudioInterface> audioInterface;

        static sys::Service *ownerService;
        static Devicei device;

        static btstack_packet_callback_registration_t hciEventCallbackRegistration;
        static hfp_ag_indicator_t agIndicators[];
        static const char *callHoldServices[];
        static hfp_generic_status_indicator_t hfIndicators[];
    };
} // namespace bluetooth
