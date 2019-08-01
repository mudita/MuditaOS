#include "WorkerImpl.hpp"

// #define __BTSTACK_FILE__ ".c"

extern "C" {
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "btstack_config.h"

#include <btstack_debug.h>
#include <btstack_event.h>
#include <btstack_link_key_db_fs.h>
#include <btstack_memory.h>
#include <btstack_run_loop.h>
#include <btstack_run_loop_freertos.h>
#include <bluetooth_company_id.h>
#include <hci.h>
#include <hci_dump.h>
#include <btstack_stdin.h>
#include <btstack_tlv_posix.h>

#include <btstack_chipset_cc256x.h>
};

#include "Bluetooth/profiles/GAP.hpp"

#define TLV_DB_PATH_PREFIX "/tmp/btstack_"
#define TLV_DB_PATH_POSTFIX ".tlv"
static char tlv_db_path[100];
static const btstack_tlv_t * tlv_impl;
static btstack_tlv_posix_t   tlv_context;

static void local_version_information_handler(uint8_t * packet);

static hci_transport_config_uart_t config = {
    HCI_TRANSPORT_CONFIG_UART,
    115200,
    0,  // main baudrate
    1,  // flow control
    NULL,
};

static btstack_packet_callback_registration_t hci_event_callback_registration;

static void hci_packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
    bd_addr_t addr;
    if (packet_type != HCI_EVENT_PACKET) return;
    switch (hci_event_packet_get_type(packet)){
        case BTSTACK_EVENT_STATE:
            if (btstack_event_state_get_state(packet) != HCI_STATE_WORKING) break;
            gap_local_bd_addr(addr);
            printf("BTstack up and running at %s\n",  bd_addr_to_str(addr));
            // setup TLV
            strcpy(tlv_db_path, TLV_DB_PATH_PREFIX);
            strcat(tlv_db_path, bd_addr_to_str(addr));
            strcat(tlv_db_path, TLV_DB_PATH_POSTFIX);
            tlv_impl = btstack_tlv_posix_init_instance(&tlv_context, tlv_db_path);
            btstack_tlv_set_instance(tlv_impl, &tlv_context);
            break;
        case HCI_EVENT_COMMAND_COMPLETE:
            if (HCI_EVENT_IS_COMMAND_COMPLETE(packet, hci_read_local_name)){
                if (hci_event_command_complete_get_return_parameters(packet)[0]) break;
                // terminate, name 248 chars
                packet[6+248] = 0;
                printf("Local name: %s\n", &packet[6]);
            }
            if (HCI_EVENT_IS_COMMAND_COMPLETE(packet, hci_read_local_version_information)){
                local_version_information_handler(packet);
            }
            break;
        default:
            break;
    }
}

static void sigint_handler(int param){
    UNUSED(param);

    printf("CTRL-C - SIGINT received, shutting down..\n");
    log_info("sigint_handler: shutting down");

    // reset anyway
    btstack_stdin_reset();

    // power down
    hci_power_control(HCI_POWER_OFF);
    hci_close();
    log_info("Good bye, see you.\n");
    exit(0);
}

static int led_state = 0;
void hal_led_toggle(void){
    led_state = 1 - led_state;
    printf("LED State %u\n", led_state);
}
static void use_fast_uart(void){
#if defined(HAVE_POSIX_B240000_MAPPED_TO_3000000) || defined(HAVE_POSIX_B600_MAPPED_TO_3000000)
    printf("Using 3000000 baud.\n");
    config.baudrate_main = 3000000;
#elif defined(HAVE_POSIX_B1200_MAPPED_TO_2000000) || defined(HAVE_POSIX_B300_MAPPED_TO_2000000)
    printf("Using 2000000 baud.\n");
    config.baudrate_main = 2000000;
#else
    printf("Using 921600 baud.\n");
    config.baudrate_main = 921600;
#endif
}

static void local_version_information_handler(uint8_t * packet){
    printf("Local version information:\n");
    uint16_t hci_version    = packet[6];
    uint16_t hci_revision   = little_endian_read_16(packet, 7);
    uint16_t lmp_version    = packet[9];
    uint16_t manufacturer   = little_endian_read_16(packet, 10);
    uint16_t lmp_subversion = little_endian_read_16(packet, 12);
    printf("- HCI Version    0x%04x\n", hci_version);
    printf("- HCI Revision   0x%04x\n", hci_revision);
    printf("- LMP Version    0x%04x\n", lmp_version);
    printf("- LMP Subversion 0x%04x\n", lmp_subversion);
    printf("- Manufacturer 0x%04x\n", manufacturer);
    switch (manufacturer){
        case BLUETOOTH_COMPANY_ID_TEXAS_INSTRUMENTS_INC:
            printf("Texas Instruments - CC256x compatible chipset.\n");
            if (lmp_subversion != btstack_chipset_cc256x_lmp_subversion()){
                printf("Error: LMP Subversion does not match initscript! ");
                printf("Your initscripts is for %s chipset\n", btstack_chipset_cc256x_lmp_subversion() < lmp_subversion ? "an older" : "a newer");
                printf("Please update Makefile to include the appropriate bluetooth_init_cc256???.c file\n");
                exit(10);
            }
            use_fast_uart();
            hci_set_chipset(btstack_chipset_cc256x_instance());
#ifdef ENABLE_EHCILL
            printf("eHCILL enabled.\n");
#else
            printf("eHCILL disable.\n");
#endif

            break;
        case BLUETOOTH_COMPANY_ID_NORDIC_SEMICONDUCTOR_ASA:
            printf("Nordic Semiconductor nRF5 chipset.\n");
            break;
        default:
            printf("Unknown manufacturer / manufacturer not supported yet.\n");
            break;
    }
}

void run_btstack(void*)
{
    printf("start BtStack\n");
    btstack_run_loop_execute();
}

BluetoothWorker::Error WorkerImpl::initialize_stack()
{
    Error err= SuccessBt;
    btstack_memory_init();
    btstack_run_loop_init(btstack_run_loop_freertos_get_instance());

    const char *pklg_path = "/tmp/hci_dump.pklg";
    hci_dump_open(pklg_path, HCI_DUMP_PACKETLOGGER);
    printf("Packet Log: %s\n", pklg_path);

    //// TODO check
    config.device_name = "/dev/ttyUSB0";
    printf("H4 device: %s\n", config.device_name);

    // init HCI
    const btstack_uart_block_t *uart_driver = btstack_uart_block_posix_instance();
    const hci_transport_t *transport = hci_transport_h4_instance(uart_driver);
    const btstack_link_key_db_t *link_key_db = btstack_link_key_db_fs_instance();
    hci_init(transport, (void *)&config);
    hci_set_link_key_db(link_key_db);

    hci_event_callback_registration.callback = &hci_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    // signal(SIGINT, sigint_handler);

    xTaskCreate(run_btstack, std::string("BtStack").c_str(), 8192, NULL, tskIDLE_PRIORITY, &handle);
    return err;
}

WorkerImpl::WorkerImpl() : handle(NULL)
{
    initialize_stack();
}

WorkerImpl::~WorkerImpl() {
    // TODO destroy task in handle
}

WorkerImpl *WorkerImpl::create()
{
    return new WorkerImpl();
}
