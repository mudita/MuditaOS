// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
    enum ScanState
    {
        init,
        active,
        done
    };
    class GAP
    {
        static std::vector<Devicei> devices;
        static sys::Service *ownerService;
        static btstack_packet_callback_registration_t cb_handler;
        static constexpr auto inquiryIntervalSeconds = 5;
        static ScanState state;
        static void sendDevices();
        static auto startScan() -> int;
        static auto remoteNameToFetch() -> bool;
        static void fetchRemoteName();
        static void continueScanning();
        static auto updateDeviceName(std::uint8_t *packet, bd_addr_t &addr) -> bool;
        static void addNewDevice(std::uint8_t *packet, bd_addr_t &addr);
        static void activeStateHandler(std::uint8_t eventType, std::uint8_t *packet, bd_addr_t &addr);
        static void packetHandler(std::uint8_t packet_type,
                                  std::uint16_t channel,
                                  std::uint8_t *packet,
                                  std::uint16_t size);
        static void processInquiryResult(std::uint8_t *packet, bd_addr_t &addr);
        static void processInquiryComplete();
        static void processNameRequestComplete(std::uint8_t *packet, bd_addr_t &addr);
        static void processDedicatedBondingCompleted(std::uint8_t *packet, bd_addr_t &addr);
        static void initStateHandler(std::uint8_t eventType, std::uint8_t *packet);

      public:
        /// THIS have to be called prior to Bt system start!
        static auto registerScan() -> Error;
        auto scan() -> Error;
        void stopScan();
        void setVisibility(bool visibility);
        auto pair(uint8_t *addr, std::uint8_t protectionLevel = 0) -> bool;
        auto unpair(uint8_t *addr) -> bool;
        static auto getDevicesList() -> const std::vector<Devicei> &;
        static auto isServiceSupportedByRemote(bd_addr_t addr, uint32_t typeOfService) -> bool;
        static void respondPinCode(const std::string &pin);

        static std::string currentlyProcessedDeviceAddr;
        explicit GAP(sys::Service *owner);
    };
} // namespace bluetooth
