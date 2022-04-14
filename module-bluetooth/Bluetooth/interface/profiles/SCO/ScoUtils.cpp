// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ScoUtils.hpp"
#include <log/log.hpp>

extern "C"
{
#include <module-bluetooth/lib/btstack/src/btstack_defines.h>
#include <module-bluetooth/lib/btstack/src/hci.h>
}

namespace bluetooth::sco::utils
{
    void sendZeros(hci_con_handle_t scoHandle)
    {
        if (scoHandle == HCI_CON_HANDLE_INVALID) {
            LOG_ERROR("Invalid scoHandle!");
            return;
        }
        constexpr auto packetDataOffset = 3;
        constexpr auto lengthPosition = 2;
        const auto scoPacketLength    = hci_get_sco_packet_length();
        const auto scoPayloadLength     = scoPacketLength - packetDataOffset;

        hci_reserve_packet_buffer();
        auto scoPacket     = hci_get_outgoing_packet_buffer();
        auto scoPayloadPtr = scoPacket + packetDataOffset;

        memset(scoPayloadPtr, 0, scoPayloadLength); // zero-fill the payload

        little_endian_store_16(scoPacket, 0, scoHandle);
        scoPacket[lengthPosition] = scoPayloadLength;

        hci_send_sco_packet_buffer(scoPacketLength);
        hci_request_sco_can_send_now_event();
    }
} // namespace bluetooth::sco::utils
