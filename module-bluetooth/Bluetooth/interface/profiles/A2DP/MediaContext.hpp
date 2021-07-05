// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

extern "C"
{
#include <btstack.h>
#include <btstack_defines.h>
}

#include <cstdint>

namespace bluetooth
{
    static constexpr int SBC_STORAGE_SIZE = 1030;
    struct MediaContext
    {
        uint16_t a2dp_cid;
        uint8_t local_seid;
        uint8_t remote_seid;
        uint8_t stream_opened;
        uint16_t avrcp_cid;

        uint32_t time_audio_data_sent_in_ms;
        uint32_t acc_num_missed_samples;
        uint32_t samples_ready;
        btstack_timer_source_t audio_timer;
        uint8_t streaming;
        int max_media_payload_size;

        uint8_t sbc_storage[SBC_STORAGE_SIZE];
        uint16_t sbc_storage_count;
        uint8_t sbc_ready_to_send;

        uint16_t volume;
    };

} // namespace bluetooth
