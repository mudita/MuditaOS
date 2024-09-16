// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include <bsp/bluetooth/Bluetooth.hpp>
#include <log/log.hpp>

using namespace bsp;

extern "C"
{
#include "btstack_uart_block_rt1051.h"
#include <hci_transport.h>
#include <btstack_run_loop.h>
#include <btstack_uart_block.h>
#include <btstack_run_loop_freertos.h>
}

// #define DEBUG_UART

namespace
{
    // and it's hci_transport_config_uart_t which is a bit different...
    int uart_rt1051_init([[maybe_unused]] const btstack_uart_config_t *config)
    {
        LOG_INFO("Create BlueKitchen interface");
        BlueKitchen::getInstance();
        return 0;
    }

    int uart_rt1051_open()
    {
        LOG_INFO("BlueKitchen UART open");
        BlueKitchen::getInstance().open();
        return 0;
    }

    int uart_rt1051_close()
    {
        LOG_INFO("BlueKitchen UART close");
        BlueKitchen::getInstance().close();
        return 0;
    }

    void uart_rt1051_set_block_received(void (*handler)())
    {
        BlueKitchen::getInstance().readReadyCallback = handler;
    }

    void uart_rt1051_set_block_sent(void (*handler)())
    {
        BlueKitchen::getInstance().writeDoneCallback = handler;
    }

    int uart_rt1051_set_baudrate(std::uint32_t baudrate)
    {
        BlueKitchen::getInstance().setBaudrate(baudrate);
        return 0;
    }

    int uart_rt1051_set_parity(int parity)
    {
        // Not implemented
        LOG_INFO("BlueKitchen set pairity: %d", parity);
        return 0;
    }

    void uart_rt1051_receive_block(std::uint8_t *buffer, std::uint16_t len)
    {
#ifdef DEBUG_UART
        LOG_DEBUG("<-- read: %d", len);
#endif
        BlueKitchen::getInstance().read(buffer, len);
    }

    void uart_rt1051_send_block(const uint8_t *buffer, uint16_t length)
    {
#ifdef DEBUG_UART
        LOG_DEBUG("--> write: %d", length);
#endif
        BlueKitchen::getInstance().write(buffer, length);
    }

    int btstack_uart_rt1051_get_supported_sleep_modes()
    {
        return BTSTACK_UART_SLEEP_MASK_RTS_HIGH_WAKE_ON_CTS_PULSE;
    }

    void btstack_uart_rt1051_set_sleep([[maybe_unused]] btstack_uart_sleep_mode_t sleep_mode)
    {}

    void btstack_uart_rt1051_set_wakeup_handler([[maybe_unused]] void (*wakeup_handler)())
    {}

    const btstack_uart_block_t btstack_uart_posix = {
        /* int  (*init)(hci_transport_config_uart_t * config); */ uart_rt1051_init,
        /* int  (*open)(void); */ uart_rt1051_open,
        /* int  (*close)(void); */ uart_rt1051_close,
        /* void (*set_block_received)(void (*handler)(void)); */ uart_rt1051_set_block_received,
        /* void (*set_block_sent)(void (*handler)(void)); */ uart_rt1051_set_block_sent,
        /* int  (*set_baudrate)(uint32_t baudrate); */ uart_rt1051_set_baudrate,
        /* int  (*set_parity)(int parity); */ uart_rt1051_set_parity,
        /* int  (*set_flowcontrol)(int flowcontrol); */ nullptr,
        /* void (*receive_block)(uint8_t *buffer, uint16_t len); */ uart_rt1051_receive_block,
        /* void (*send_block)(const uint8_t *buffer, uint16_t length); */ uart_rt1051_send_block,
        /* int (*get_supported_sleep_modes)(); */ btstack_uart_rt1051_get_supported_sleep_modes,
        /* void (*set_sleep)(btstack_uart_sleep_mode_t sleep_mode); */ btstack_uart_rt1051_set_sleep,
        /* void (*set_wakeup_handler)(void (*handler)(void)); */ btstack_uart_rt1051_set_wakeup_handler,
    };
} // namespace

const btstack_uart_block_t *btstack_uart_block_rt1051_instance()
{
    return &btstack_uart_posix;
}
