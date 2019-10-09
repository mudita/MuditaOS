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

namespace {  // interfacing
// static hci_transport_config_uart_t *uart_config;
// data source for integration with BTstack Runloop
static btstack_data_source_t transport_data_source;
// callbacks
static void (*block_sent)(void);
static void (*block_received)(void);
};

// and it's hci_transport_config_uart_t which is a bit different...
static int uart_rt1051_init(const btstack_uart_config_t *config)
{
    LOG_INFO("Create BlueKitchen interface");
    BlueKitchen *bt = BlueKitchen::getInstance();
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

static void uart_rt1051_set_block_received(void (*handler)(void)) {
    /// TODO read_ready_cb call from my thread
    BlueKitchen::getInstance()->read_ready_cb = handler;
}

static void uart_rt1051_set_block_sent(void (*handler)(void)) {
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

static int uart_rt1051_set_flowcontroll(int flowcontroll)
{
    LOG_INFO("BlueKitchen set flowcontroll: %d", flowcontroll);
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
    BlueKitchen::getInstance()->write_blocking((char*)buffer, length);
    /// TODO check me
}

static const btstack_uart_block_t btstack_uart_posix = {
    /* int  (*init)(hci_transport_config_uart_t * config); */         uart_rt1051_init,
    /* int  (*open)(void); */                                         uart_rt1051_open,
    /* int  (*close)(void); */                                        uart_rt1051_close,
    /* void (*set_block_received)(void (*handler)(void)); */          uart_rt1051_set_block_received,
    /* void (*set_block_sent)(void (*handler)(void)); */              uart_rt1051_set_block_sent,
    /* int  (*set_baudrate)(uint32_t baudrate); */                    uart_rt1051_set_baudrate,
    /* int  (*set_parity)(int parity); */                             uart_rt1051_set_parity,
    /* int  (*set_flowcontrol)(int flowcontrol); */                   uart_rt1051_set_flowcontroll,
    /* void (*receive_block)(uint8_t *buffer, uint16_t len); */       uart_rt1051_receive_block,
    /* void (*send_block)(const uint8_t *buffer, uint16_t length); */ uart_rt1051_send_block,
    /* int (*get_supported_sleep_modes); */                           NULL,
    /* void (*set_sleep)(btstack_uart_sleep_mode_t sleep_mode); */    NULL,
    /* void (*set_wakeup_handler)(void (*handler)(void)); */          NULL,
};

//// TODO handle me
const btstack_uart_block_t *btstack_uart_block_rt1051_instance()
{
    LOG_INFO("btstack_uart_block_rt1051_instance");
    return &btstack_uart_posix;
}

};
