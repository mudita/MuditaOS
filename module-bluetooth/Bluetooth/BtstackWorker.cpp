// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <cstdlib>
#include <log/log.hpp>

extern "C"
{
#include <btstack_event.h>

#include <bluetooth_company_id.h>
#include <btstack_memory.h>
#include <btstack_run_loop.h>
#include <btstack_stdin.h>
#include <hci.h>
#include <hci_dump.h>

#include <btstack_chipset_cc256x.h>
#include <btstack_link_key_db_memory.h>
};

#ifdef TARGET_RT1051
#include <Bluetooth/glucode/btstack_uart_block_rt1051.h>
#include <btstack_run_loop_freertos.h>
#else
extern "C"
{
#include <btstack_run_loop_posix.h>
#include <btstack_tlv_posix.h>
}
#endif

#include "BtKeysStorage.hpp"
#include <Error.hpp>
#include <functional>

// #define TLV_DB_PATH_PREFIX "/tmp/btstack_"
// #define TLV_DB_PATH_POSTFIX ".tlv"
// static char tlv_db_path[100];
// static const btstack_tlv_t * tlv_impl;
// static btstack_tlv_posix_t   tlv_context;

static void local_version_information_handler(uint8_t *packet);

static hci_transport_config_uart_t config = {
    .type          = HCI_TRANSPORT_CONFIG_UART,
    .baudrate_init = 115200,
    .baudrate_main = 0, // main baudrate
    .flowcontrol   = 1, // flow control
    .device_name   = nullptr,
};

static btstack_packet_callback_registration_t hci_event_callback_registration;

static void hci_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    bd_addr_t addr;
    if (packet_type != HCI_EVENT_PACKET) {
        return;
    }
    switch (hci_event_packet_get_type(packet)) {
    case BTSTACK_EVENT_STATE:
        if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING) {
            break;
        }
        gap_local_bd_addr(addr);
        LOG_INFO("BTstack up and running at %s", bd_addr_to_str(addr));
        // setup TLV
        //            strcpy(tlv_db_path, TLV_DB_PATH_PREFIX);
        //            strcat(tlv_db_path, bd_addr_to_str(addr));
        //            strcat(tlv_db_path, TLV_DB_PATH_POSTFIX);
        //            tlv_impl = btstack_tlv_posix_init_instance(&tlv_context, tlv_db_path);
        //            btstack_tlv_set_instance(tlv_impl, &tlv_context);
        break;
    case HCI_EVENT_COMMAND_COMPLETE:
        if (HCI_EVENT_IS_COMMAND_COMPLETE(packet, hci_read_local_name)) {
            if (hci_event_command_complete_get_return_parameters(packet)[0] != 0u)
                break;
            // terminate, name 248 chars
            packet[6 + 248] = 0;
            LOG_INFO("Local name: %s", &packet[6]);
        }
        if (HCI_EVENT_IS_COMMAND_COMPLETE(packet, hci_read_local_version_information)) {
            local_version_information_handler(packet);
        }
        break;
    default:
        break;
    }
}

static void sigint_handler(int param)
{
    UNUSED(param);

    LOG_INFO("CTRL-C - SIGINT received, shutting down..");

    // reset anyway
    btstack_stdin_reset();

    // power down
    hci_power_control(HCI_POWER_OFF);
    hci_close();
    exit(0);
}

static int led_state = 0;
void hal_led_toggle()
{
    led_state = 1 - led_state;
    LOG_INFO("LED State %u", led_state);
}
static void use_fast_uart()
{
#if defined(HAVE_POSIX_B240000_MAPPED_TO_3000000) || defined(HAVE_POSIX_B600_MAPPED_TO_3000000)
    LOG_INFO("Using 3000000 baud.");
    config.baudrate_main = 3000000;
#elif defined(HAVE_POSIX_B1200_MAPPED_TO_2000000) || defined(HAVE_POSIX_B300_MAPPED_TO_2000000)
    LOG_INFO("Using 2000000 baud.");
    config.baudrate_main = 2000000;
#else
    LOG_INFO("Using 921600 baud");
    config.baudrate_main = 921600;
#endif
}

static void local_version_information_handler(uint8_t *packet)
{
    LOG_INFO("Local version information:");
    uint16_t hci_version    = packet[6];
    uint16_t hci_revision   = little_endian_read_16(packet, 7);
    uint16_t lmp_version    = packet[9];
    uint16_t manufacturer   = little_endian_read_16(packet, 10);
    uint16_t lmp_subversion = little_endian_read_16(packet, 12);
    LOG_INFO("- HCI Version    0x%04x", hci_version);
    LOG_INFO("- HCI Revision   0x%04x", hci_revision);
    LOG_INFO("- LMP Version    0x%04x", lmp_version);
    LOG_INFO("- LMP Subversion 0x%04x", lmp_subversion);
    LOG_INFO("- Manufacturer 0x%04x", manufacturer);
    switch (manufacturer) {
    case BLUETOOTH_COMPANY_ID_TEXAS_INSTRUMENTS_INC:
        LOG_INFO("Texas Instruments - CC256x compatible chipset.");
        if (lmp_subversion != btstack_chipset_cc256x_lmp_subversion()) {
            LOG_INFO("Error: LMP Subversion does not match initscript! ");
            LOG_INFO("Your initscripts is for %s chipset",
                     btstack_chipset_cc256x_lmp_subversion() < lmp_subversion ? "an older" : "a newer");
            LOG_INFO("Please update Makefile to include the appropriate bluetooth_init_cc256???.c file");
            exit(10);
        }
//        use_fast_uart();
        hci_set_chipset(btstack_chipset_cc256x_instance());
#ifdef ENABLE_EHCILL
        LOG_INFO("eHCILL enabled.");
#else
        LOG_INFO("eHCILL disable.");
#endif

        break;
    case BLUETOOTH_COMPANY_ID_NORDIC_SEMICONDUCTOR_ASA:
        LOG_INFO("Nordic Semiconductor nRF5 chipset.");
        break;
    default:
        LOG_INFO("Unknown manufacturer / manufacturer not supported yet.\n");
        break;
    }
}

extern "C"
{
#include <FreeRTOS.h>
#include <task.h>
};

namespace Bt
{
    void run_btstack(void *)
    {
        LOG_INFO("- run BtStack loop\n");
        btstack_run_loop_execute();
    }

    auto initialize_stack() -> Error
    {
        btstack_memory_init();
#ifdef TARGET_RT1051
        btstack_run_loop_init(btstack_run_loop_freertos_get_instance());
        const btstack_uart_block_t *uart_driver = btstack_uart_block_rt1051_instance();
#else
        btstack_run_loop_init(btstack_run_loop_posix_get_instance());
        config.device_name = "/dev/telit";
        LOG_INFO("H4 device: %s", config.device_name);
        const btstack_uart_block_t *uart_driver = btstack_uart_block_posix_instance();
#endif
        const hci_transport_t *transport = hci_transport_h4_instance(uart_driver);

        hci_init(transport, (void *)&config);
        hci_set_link_key_db(KeyStorage::getKeyStorage());

        hci_event_callback_registration.callback = &hci_packet_handler;
        hci_add_event_handler(&hci_event_callback_registration);
        LOG_DEBUG("BT worker run success");
        return Error();
    }

    auto register_hw_error_callback(std::function<void(uint8_t)> new_callback) -> Error
    {
        static std::function<void(uint8_t)> callback = nullptr;
        callback                                     = new_callback;
        hci_set_hardware_error_callback([](uint8_t val) -> void {
            LOG_ERROR("Bluetooth HW ERROR! %d", val);
            if (callback) {
                callback(val);
            }
        });
        return Error();
    }

    auto run_stack(TaskHandle_t *handle) -> Error
    {
        BaseType_t taskerr = 0;
        LOG_INFO("Past last moment for Bt registration prior to RUN state");
        hci_power_control(HCI_POWER_ON);
        if ((taskerr = xTaskCreate(run_btstack, "BtStack", 1024, NULL, tskIDLE_PRIORITY, handle)) != pdPASS) {
            LOG_ERROR("BT Service failure! %lu", taskerr);
            return Error(Error::SystemError, taskerr);
        }
        return Error();
    }
} // namespace Bt
