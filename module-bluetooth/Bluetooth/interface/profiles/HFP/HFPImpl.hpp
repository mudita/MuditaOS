// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "HFP.hpp"
#include "Error.hpp"
#include <interface/profiles/SCO/SCO.hpp>
#include <Audio/AudioCommon.hpp>

namespace bluetooth
{
    static constexpr int serviceBufferLength = 150;
    static constexpr int commandBufferLength = 150;

    class HFP::HFPImpl
    {
      public:
        static void packetHandler(uint8_t packetType, uint16_t channel, uint8_t *event, uint16_t eventSize);
        auto init() -> Error::Code;
        void startRinging() const noexcept;
        void stopRinging() const noexcept;
        void initializeCall() const noexcept;
        void terminateCall() const noexcept;
        void connect();
        void disconnect();
        void setDevice(Devicei device);
        void setOwnerService(const sys::Service *service);
        auto getStreamData() -> std::shared_ptr<BluetoothStreamData>;
        void setAudioDevice(std::shared_ptr<bluetooth::BluetoothAudioDevice> audioDevice);
        [[nodiscard]] auto setupCall() const noexcept -> Error::Code;
        [[nodiscard]] auto setIncomingCallNumber(const std::string &num) const noexcept -> Error::Code;
        [[nodiscard]] auto setSignalStrength(int bars) const noexcept -> Error::Code;
        [[nodiscard]] auto setOperatorName(const std::string_view &name) const noexcept -> Error::Code;
        [[nodiscard]] auto setBatteryLevel(const BatteryLevel &level) const noexcept -> Error::Code;
        [[nodiscard]] auto setNetworkRegistrationStatus(bool registered) const noexcept -> Error::Code;
        [[nodiscard]] auto setRoamingStatus(bool enabled) const noexcept -> Error::Code;

      private:
        static void sendAudioEvent(audio::EventType event, audio::Event::DeviceState state);
        static void processHCIEvent(uint8_t *event);
        static void processHFPEvent(uint8_t *event);
        static void initCodecs();
        static void dump_supported_codecs(void);
        static std::array<uint8_t, serviceBufferLength> serviceBuffer;
        static constexpr uint8_t rfcommChannelNr = 1;
        static const std::string_view agServiceName;
        static hci_con_handle_t scoHandle;
        static hci_con_handle_t aclHandle;
        static std::unique_ptr<SCO> sco;
        static std::unique_ptr<CellularInterface> cellularInterface;
        static std::unique_ptr<AudioInterface> audioInterface;
        static const sys::Service *ownerService;
        static SCOCodec codec;
        [[maybe_unused]] static int memory_1_enabled;
        static btstack_packet_callback_registration_t hci_event_callback_registration;
        [[maybe_unused]] static int ag_indicators_nr;
        static hfp_ag_indicator_t ag_indicators[7];
        [[maybe_unused]] static int call_hold_services_nr;
        static const char *call_hold_services[5];
        [[maybe_unused]] static int hf_indicators_nr;
        [[maybe_unused]] static hfp_generic_status_indicator_t hf_indicators[2];
        static std::shared_ptr<CVSDAudioDevice> audioDevice;
        static Devicei device;
        static bool isConnected;
        static bool isIncomingCall;
        static bool isCallInitialized;
    };
} // namespace bluetooth
