// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <bsp/bluetooth/Bluetooth.hpp>
#include <log/log.hpp>

using namespace bsp;

extern "C"
{

#include "btstack_uart_block_rt1051.h"
#include <hci_transport.h>
#include <btstack_run_loop.h>
#include <btstack_uart_block.h>
#include <stddef.h> // for null
#include <btstack_run_loop_freertos.h>

    // #define DEBUG_UART

    // and it's hci_transport_config_uart_t which is a bit different...
    static int uart_rt1051_init(const btstack_uart_config_t *config)
    {
        LOG_INFO("Create BlueKitchen interface");
        BlueKitchen::getInstance();
        return 0;
    }

    static int uart_rt1051_open()
    {
        LOG_INFO("BlueKitchen uart open");
        BlueKitchen::getInstance()->open();
        return 0;
    }

    static int uart_rt1051_close()
    {
        LOG_INFO("BlueKitchen uart close");
        BlueKitchen::getInstance()->close();
        return 0;
    }

    static void uart_rt1051_set_block_received(void (*handler)(void))
    {
        BlueKitchen::getInstance()->read_ready_cb = handler;
    }

    static void uart_rt1051_set_block_sent(void (*handler)(void))
    {
        BlueKitchen::getInstance()->write_done_cb = handler;
    }

    static int uart_rt1051_set_baudrate(uint32_t baudrate)
    {
        BlueKitchen::getInstance()->set_baudrate(baudrate);
        return 0;
    }

    static int uart_rt1051_set_parity(int pairity)
    {
        // Not implemented
        LOG_INFO("BlueKitchen set pairity: %d", pairity);
        return 0;
    }

    static int uart_rt1051_set_flowcontrol(int flowcontrol)
    {
        LOG_INFO("BlueKitchen set flowcontrol: %d", flowcontrol);
        // BlueKitchen::getInstance()->set_rts(); ??
        return 0;
    }

    static void uart_rt1051_receive_block(uint8_t *buffer, uint16_t len)
    {
#ifdef DEBUG_UART
        LOG_INFO("<-- read: %d", len);
#endif
        BlueKitchen::getInstance()->read(buffer, len);
    }

    static void uart_rt1051_send_block(const uint8_t *buffer, uint16_t length)
    {
#ifdef DEBUG_UART
        LOG_INFO("--> write: %d", length);
#endif
        BlueKitchen::getInstance()->write(buffer, length);
    }

    static int btstack_uart_rt1051_get_supported_sleep_modes(void)
    {
        return BTSTACK_UART_SLEEP_MASK_RTS_HIGH_WAKE_ON_CTS_PULSE;
    }

    static void btstack_uart_rt1051_set_sleep(btstack_uart_sleep_mode_t sleep_mode)
    {}

    static void btstack_uart_rt1051_set_wakeup_handler(void (*the_wakeup_handler)(void))
    {}

    static const btstack_uart_block_t btstack_uart_posix = {
        /* int  (*init)(hci_transport_config_uart_t * config); */ uart_rt1051_init,
        /* int  (*open)(void); */ uart_rt1051_open,
        /* int  (*close)(void); */ uart_rt1051_close,
        /* void (*set_block_received)(void (*handler)(void)); */ uart_rt1051_set_block_received,
        /* void (*set_block_sent)(void (*handler)(void)); */ uart_rt1051_set_block_sent,
        /* int  (*set_baudrate)(uint32_t baudrate); */ uart_rt1051_set_baudrate,
        /* int  (*set_parity)(int parity); */ uart_rt1051_set_parity,
        /* int  (*set_flowcontrol)(int flowcontrol); */ NULL, // uart_rt1051_set_flowcontrol,
        /* void (*receive_block)(uint8_t *buffer, uint16_t len); */ uart_rt1051_receive_block,
        /* void (*send_block)(const uint8_t *buffer, uint16_t length); */ uart_rt1051_send_block,
        /* int (*get_supported_sleep_modes); */ &btstack_uart_rt1051_get_supported_sleep_modes,
        /* void (*set_sleep)(btstack_uart_sleep_mode_t sleep_mode); */ &btstack_uart_rt1051_set_sleep,
        /* void (*set_wakeup_handler)(void (*handler)(void)); */ &btstack_uart_rt1051_set_wakeup_handler,
    };

    const btstack_uart_block_t *btstack_uart_block_rt1051_instance()
    {
        LOG_INFO("btstack_uart_block_rt1051_instance");
        return &btstack_uart_posix;
    }
};
