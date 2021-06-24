// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BluetoothDriver.hpp"

extern "C"
{
#include <btstack.h>
#include <hci_transport.h>
#include <hci_transport_h4.h>
}
#include "GAP/GAP.hpp"

namespace bluetooth
{
    class Driver : public AbstractDriver
    {
      private:
        static hci_transport_config_uart_t config;
        const btstack_run_loop *runLoop;
        btstack_packet_callback_registration_t hci_event_callback_registration;
        std::unique_ptr<bluetooth::GAP> gap;
        static void hci_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
        static void local_version_information_handler(uint8_t *packet);
#ifdef TARGET_RT1051
        [[maybe_unused]] auto runLoopInitTarget(const btstack_run_loop *runLoop) -> const btstack_uart_block_t *;
#else
        [[maybe_unused]] auto runLoopInitLinux(const btstack_run_loop *runLoop) -> const btstack_uart_block_t *;
#endif

      public:
        Driver(const btstack_run_loop *runLoop, sys::Service *ownerService);

        [[nodiscard]] auto init() -> Error::Code override;
        [[nodiscard]] auto run() -> Error::Code override;
        [[nodiscard]] auto stop() -> Error::Code override;
        void registerErrorCallback(const ErrorCallback &newCallback) override;
        auto scan() -> Error override;
        void stopScan() override;
        void setVisibility(bool visibility) override;
        auto pair(uint8_t *addr, std::uint8_t protectionLevel = 0) -> bool override;
        auto unpair(uint8_t *addr) -> bool override;
    };
} // namespace bluetooth
