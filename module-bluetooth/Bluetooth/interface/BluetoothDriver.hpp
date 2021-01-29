// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <btstack.h>
#include <functional>
#include "Error.hpp"

namespace bluetooth
{
    class AbstractDriver
    {
      public:
        using ErrorCallback                = std::function<void(uint8_t)>;
        virtual ~AbstractDriver() noexcept = default;

        [[nodiscard]] virtual auto init() -> Error::Code                     = 0;
        [[nodiscard]] virtual auto run() -> Error::Code                      = 0;
        [[nodiscard]] virtual auto stop() -> Error::Code                     = 0;
        virtual void registerErrorCallback(const ErrorCallback &newCallback) = 0;
    };

    class Driver : public AbstractDriver
    {
      private:
        static hci_transport_config_uart_t config;
        const btstack_run_loop *runLoop;
        btstack_packet_callback_registration_t hci_event_callback_registration;
        static void hci_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
        static void local_version_information_handler(uint8_t *packet);
#ifdef TARGET_RT1051
        [[maybe_unused]] auto runLoopInitTarget(const btstack_run_loop *runLoop) -> const btstack_uart_block_t *;
#else
        [[maybe_unused]] auto runLoopInitLinux(const btstack_run_loop *runLoop) -> const btstack_uart_block_t *;
#endif

      public:
        explicit Driver(const btstack_run_loop *runLoop);

        [[nodiscard]] auto init() -> Error::Code override;
        [[nodiscard]] auto run() -> Error::Code override;
        [[nodiscard]] auto stop() -> Error::Code override;
        void registerErrorCallback(const ErrorCallback &newCallback) override;
    };
} // namespace bluetooth
