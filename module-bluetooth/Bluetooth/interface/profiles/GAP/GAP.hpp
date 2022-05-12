// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <vector>
#include <Bluetooth/Device.hpp>
#include <Bluetooth/Error.hpp>
#include <Service/Service.hpp>
extern "C"
{
#include "btstack_defines.h"
}
namespace bluetooth
{
    namespace stack
    {
        enum state
        {
            off,
            init,
            working,
            halting,
            sleeping,
            falling_asleep
        };
    }

    class GAP
    {
        static sys::Service *ownerService;
        static btstack_packet_callback_registration_t cb_handler;
        static constexpr auto inquiryIntervalSeconds = 5;
        static stack::state state;
        static void sendDevices();
        static auto startScan() -> int;
        static void continueScanning();
        static auto updateDeviceName(std::uint8_t *packet, bd_addr_t &addr) -> bool;
        static void addNewDevice(std::uint8_t *packet, bd_addr_t &addr);
        static void activeStateHandler(std::uint8_t eventType, std::uint8_t *packet, std::uint16_t size);
        static void packetHandler(std::uint8_t packet_type,
                                  std::uint16_t channel,
                                  std::uint8_t *packet,
                                  std::uint16_t size);
        static void processInquiryResult(std::uint8_t *packet);
        static void processInquiryComplete();
        static void processNameRequestComplete(std::uint8_t *packet, bd_addr_t &addr);
        static void processDedicatedBondingCompleted(std::uint8_t *packet, bd_addr_t &addr);
        static void processSimplePairingCompleted(std::uint8_t *packet, bd_addr_t &addr);
        static void initStateHandler(std::uint8_t eventType, std::uint8_t *packet);
        static auto getDeviceIndexForAddress(const std::vector<Devicei> &devs, const bd_addr_t addr) -> int;

      public:
        /// THIS have to be called prior to Bt system start!
        static auto registerScan() -> Error;
        auto scan() -> Error;
        void stopScan();
        void setVisibility(bool visibility);
        void pair(Devicei device, std::uint8_t protectionLevel = 0);
        void unpair(Devicei device);
        static auto getDevicesList() -> std::vector<Devicei>;
        static void respondPinCode(const std::string &pin, Devicei d);
        static void finishCodeComparison(bool accepted, Devicei d);
        static Devicei currentlyProccesedDevice;
        explicit GAP(sys::Service *owner);
    };
} // namespace bluetooth
