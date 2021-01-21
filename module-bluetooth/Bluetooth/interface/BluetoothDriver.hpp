// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <btstack.h>
#include <functional>
#include "Error.hpp"

namespace bluetooth
{

    class Driver
    {
      private:
        static hci_transport_config_uart_t config;
        btstack_packet_callback_registration_t hci_event_callback_registration;
        static void hci_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
        static void local_version_information_handler(uint8_t *packet);
        [[maybe_unused]] auto runLoopInitTarget(const btstack_run_loop *runLoop) -> const btstack_uart_block_t *;
        [[maybe_unused]] auto runLoopInitLinux(const btstack_run_loop *runLoop) -> const btstack_uart_block_t *;

      public:
        auto init(const btstack_run_loop *runLoop) -> Error::Code;
        auto run() -> Error::Code;
        auto stop() -> Error::Code;
        void registerHardwareErrorCallback(std::function<void(uint8_t)> new_callback);
    };
} // namespace bluetooth
