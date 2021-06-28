// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log.hpp>

extern "C"
{
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "module-bluetooth/Bluetooth/btstack_config.h"

#include <btstack_event.h>
// #include <btstack_link_key_db_fs.h>
#include <bluetooth_company_id.h>
#include <btstack_memory.h>
#include <btstack_run_loop.h>
#include <btstack_run_loop_freertos.h>
#include <btstack_stdin.h>
#include <hci.h>
#include <hci_dump.h>
    // #include <btstack_tlv_posix.h>

#include <bluetooth_sdp.h>
#include <bnep_lwip.h>
#include <btstack_chipset_cc256x.h>
#include <pan.h>
#include <sdp_util.h>
};

#include <BtCommand.hpp>
#include <Error.hpp>
#ifdef TARGET_RT1051
#include <Bluetooth/glucode/btstack_uart_block_rt1051.h>
#endif

static btstack_packet_callback_registration_t hci_event_callback_registration;

#define NETWORK_TYPE_IPv4 0x0800
#define NETWORK_TYPE_ARP  0x0806
#define NETWORK_TYPE_IPv6 0x86DD

static uint8_t pan_sdp_record[220];

static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    /* LISTING_PAUSE */
    UNUSED(channel);
    UNUSED(size);

    bd_addr_t event_addr;

    switch (packet_type) {
    case HCI_EVENT_PACKET:
        switch (hci_event_packet_get_type(packet)) {

        case HCI_EVENT_PIN_CODE_REQUEST:
            // inform about pin code request
            LOG_INFO("Pin code request");
            hci_event_pin_code_request_get_bd_addr(packet, event_addr);
            gap_pin_code_response(event_addr, "0000");
            break;

        case HCI_EVENT_USER_CONFIRMATION_REQUEST:
            // inform about user confirmation request
            LOG_INFO("SSP User Confirmation Auto accept");
            hci_event_user_confirmation_request_get_bd_addr(packet, event_addr);
            break;

        /* @text BNEP_EVENT_CHANNEL_OPENED is received after a BNEP connection was established or
         * or when the connection fails. The status field returns the error code.
         */
        case BNEP_EVENT_CHANNEL_OPENED:
            if (bnep_event_channel_opened_get_status(packet)) {
                LOG_INFO("BNEP channel open failed, status %02x", bnep_event_channel_opened_get_status(packet));
            }
            else {
                uint16_t uuid_source = bnep_event_channel_opened_get_source_uuid(packet);
                uint16_t uuid_dest   = bnep_event_channel_opened_get_destination_uuid(packet);
                uint16_t mtu         = bnep_event_channel_opened_get_mtu(packet);
                bnep_event_channel_opened_get_remote_address(packet, event_addr);
                LOG_INFO("BNEP connection open succeeded source UUID 0x%04x dest UUID: 0x%04x, max frame size %u",
                         uuid_source,
                         uuid_dest,
                         mtu);
                LOG_INFO("Please open 'http://192.168.7.1' in your web browser: ");
            }
            break;

        /* @text BNEP_EVENT_CHANNEL_CLOSED is received when the connection gets closed.
         */
        case BNEP_EVENT_CHANNEL_CLOSED:
            LOG_INFO("BNEP channel closed");
            break;

        default:
            break;
        }
        break;
    default:
        break;
    }
}

namespace bluetooth
{

    // Set local name with a template Bluetooth address, that will be automatically
    // replaced with a actual address once it is available, i.e. when BTstack boots
    Error set_name(std::string &name)
    {
        // name has to have storage
        constexpr uint32_t size = 64;
        static std::array<char, size> lname;
        snprintf(lname.data(), size, "%s", name.c_str());
        gap_set_local_name(lname.data());
        return Error();
    }
    namespace PAN
    {

        Error bnep_setup()
        {
            // Discoverable
            // up and starts talking to a Bluetooth module.
            gap_discoverable_control(1);

            // register for HCI events
            hci_event_callback_registration.callback = &packet_handler;
            hci_add_event_handler(&hci_event_callback_registration);

            // Initialize L2CAP
            l2cap_init();

            // Initialize BNEP
            bnep_init();

            // Init SDP
            sdp_init();
            memset(pan_sdp_record, 0, sizeof(pan_sdp_record));
            uint16_t network_packet_types[] = {NETWORK_TYPE_IPv4, NETWORK_TYPE_ARP, 0}; // 0 as end of list

            // NAP Network Access Type: Other, 1 MB/s
            pan_create_nap_sdp_record(pan_sdp_record,
                                      sdp_create_service_record_handle(),
                                      network_packet_types,
                                      NULL,
                                      NULL,
                                      BNEP_SECURITY_NONE,
                                      PAN_NET_ACCESS_TYPE_OTHER,
                                      1000000,
                                      NULL,
                                      NULL);
            sdp_register_service(pan_sdp_record);
            LOG_INFO("SDP service record size: %u", de_get_len((uint8_t *)pan_sdp_record));
            return Error();
        }

        Error bnep_start()
        {
            bnep_lwip_init();

            // Setup NAP Service via BENP lwIP adapter
            bnep_lwip_register_service(BLUETOOTH_SERVICE_CLASS_NAP, 1691);

            // register callback - to print state
            bnep_lwip_register_packet_handler(packet_handler);
            return Error();
        }

    } // namespace PAN
} // namespace Bt
