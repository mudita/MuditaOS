// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
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
        btstack_packet_callback_registration_t hci_event_callback_registration;
        std::unique_ptr<bluetooth::GAP> gap;
        static PowerOnCallback powerOnCallback;

        static void hci_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
        static void local_version_information_handler(uint8_t *packet);
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
