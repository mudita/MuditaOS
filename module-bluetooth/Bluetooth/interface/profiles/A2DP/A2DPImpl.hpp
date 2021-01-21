// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include "A2DP.hpp"
#include <Bluetooth/Device.hpp>
#include <Bluetooth/Error.hpp>
#include <BtCommand.hpp>
#include <log/log.hpp>
#include <Audio/AudioCommon.hpp>

extern "C"
{
#include <btstack.h>
#include <btstack_defines.h>
}

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

    class AVRCP;
    class A2DP::A2DPImpl
    {

        static constexpr int NUM_CHANNELS           = 2;
        static constexpr int BYTES_PER_AUDIO_SAMPLE = (2 * NUM_CHANNELS);
        static constexpr int AUDIO_TIMEOUT_MS       = 10;
        static constexpr int TABLE_SIZE_441HZ       = 100;
        static constexpr int SDP_BUFFER_LENGTH      = 150;
        static constexpr int MEDIA_CAP_SIZE         = 4;

        static std::array<uint8_t, SDP_BUFFER_LENGTH> sdpSourceServiceBuffer;
        static bd_addr_t deviceAddr;
        static btstack_packet_callback_registration_t hciEventCallbackRegistration;
        static std::array<uint8_t, MEDIA_CAP_SIZE> mediaSbcCodecCapabilities;
        static const sys::Service *ownerService;
        static QueueHandle_t sourceQueue;
        static QueueHandle_t sinkQueue;
        static DeviceMetadata_t metadata;

        static void startTimer(MediaContext *context);
        static void stopTimer(MediaContext *context);
        static void audioTimeoutHandler(btstack_timer_source_t *timer);
        static void sourcePacketHandler(uint8_t packetType, uint16_t channel, uint8_t *packet, uint16_t size);
        static void hciPacketHandler(uint8_t packetType, uint16_t channel, uint8_t *packet, uint16_t size);
        static void sendMediaPacket();
        static auto fillSbcAudioBuffer(MediaContext *context) -> int;
        static void sendAudioEvent(audio::EventType event, audio::Event::DeviceState state);

      public:
        auto init() -> Error::Code;
        void start();
        void stop();
        void setDeviceAddress(bd_addr_t addr);
        void setOwnerService(const sys::Service *service);
        auto getStreamData() -> std::shared_ptr<BluetoothStreamData>;
    };
} // namespace Bt
