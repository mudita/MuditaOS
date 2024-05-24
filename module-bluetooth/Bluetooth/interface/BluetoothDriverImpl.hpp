// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BluetoothDriver.hpp"
#include "GAP/GAP.hpp"

extern "C"
{
#include <btstack.h>
#include <hci_transport.h>
#include <hci_transport_h4.h>
}

namespace bluetooth
{
    class Driver : public AbstractDriver
    {
      private:
        static hci_transport_config_uart_t config;
        const btstack_run_loop *runLoop;
        btstack_packet_callback_registration_t hciEventCallbackRegistration;
        std::unique_ptr<bluetooth::GAP> gap;
        static PowerOnCallback powerOnCallback;

        static void hciPacketHandler(std::uint8_t packet_type,
                                     std::uint16_t channel,
                                     std::uint8_t *packet,
                                     std::uint16_t size);
        static void localVersionInformationHandler(std::uint8_t *packet);
#ifdef TARGET_RT1051
        [[maybe_unused]] auto runLoopInitTarget(const btstack_run_loop *runLoop) -> const btstack_uart_block_t *;
#else
        [[maybe_unused]] auto runLoopInitLinux(const btstack_run_loop *runLoop) -> const btstack_uart_block_t *;
#endif

      public:
        Driver(const btstack_run_loop *runLoop, sys::Service *ownerService);

        [[nodiscard]] auto init() -> Result::Code override;
        [[nodiscard]] auto run() -> Result::Code override;
        [[nodiscard]] auto stop() -> Result::Code override;
        void registerErrorCallback(const ErrorCallback &newCallback) override;
        void registerPowerOnCallback(const PowerOnCallback &newCallback) override;

        auto scan() -> Result override;
        void stopScan() override;
        void setVisibility(bool visibility) override;
        void pair(Devicei device, std::uint8_t protectionLevel = 0) override;
        void unpair(Devicei device) override;
    };
} // namespace bluetooth
