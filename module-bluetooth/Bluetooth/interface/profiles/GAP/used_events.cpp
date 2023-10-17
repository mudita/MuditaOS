// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "used_events.hpp"
#include <module-bluetooth/lib/btstack/src/btstack_event.h>

namespace bluetooth
{
    const char *evt_cstr(int evt)
    {
        switch (evt) {
        // ----------
        // HCI EVENTS
        // ----------
        case HCI_EVENT_TRANSPORT_PACKET_SENT:
            return "HCI_EVENT_TRANSPORT_PACKET_SENT";
        case HCI_EVENT_COMMAND_COMPLETE:
            return "HCI_EVENT_COMMAND_COMPLETE";
        case HCI_EVENT_COMMAND_STATUS:
            return "HCI_EVENT_COMMAND_STATUS";
        case HCI_EVENT_INQUIRY_COMPLETE:
            return "HCI_EVENT_INQUIRY_COMPLETE";
        case HCI_EVENT_EXTENDED_INQUIRY_RESPONSE:
            return "HCI_EVENT_EXTENDED_INQUIRY_RESPONSE";
        case HCI_EVENT_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE:
            return "HCI_EVENT_READ_REMOTE_SUPPORTED_FEATURES_COMPLETE";
        case HCI_EVENT_USER_CONFIRMATION_REQUEST:
            return "HCI_EVENT_USER_CONFIRMATION_REQUEST";
        case HCI_EVENT_PIN_CODE_REQUEST:
            return "HCI_EVENT_PIN_CODE_REQUEST";
        case HCI_EVENT_READ_REMOTE_EXTENDED_FEATURES_COMPLETE:
            return "HCI_EVENT_READ_REMOTE_EXTENDED_FEATURES_COMPLETE";
        case HCI_EVENT_USER_PASSKEY_REQUEST:
            return "HCI_EVENT_USER_PASSKEY_REQUEST";
        case HCI_EVENT_REMOTE_NAME_REQUEST_COMPLETE:
            return "HCI_EVENT_REMOTE_NAME_REQUEST_COMPLETE";
        case HCI_EVENT_SIMPLE_PAIRING_COMPLETE:
            return "HCI_EVENT_SIMPLE_PAIRING_COMPLETE";

        // ----------
        // GAP EVENTS
        // ----------
        case GAP_EVENT_PAIRING_COMPLETE:
            return "GAP_EVENT_PAIRING_COMPLETE";
        case GAP_EVENT_DEDICATED_BONDING_COMPLETED:
            return "GAP_EVENT_DEDICATED_BONDING_COMPLETED";
        case GAP_EVENT_INQUIRY_RESULT:
            return "GAP_EVENT_INQUIRY_RESULT";
        case GAP_EVENT_INQUIRY_COMPLETE:
            return "GAP_EVENT_INQUIRY_COMPLETE";
        case GAP_EVENT_PAIRING_STARTED:
            return "GAP_EVENT_PAIRING_STARTED";
        default:
            return "EVENT_UNKNOWN";
        }
    }
} // namespace bluetooth
