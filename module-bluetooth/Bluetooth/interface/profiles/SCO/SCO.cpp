// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SCO.hpp"
#include <cstdio>
#include <Audio/AudioCommon.hpp>
#include <service-evtmgr/Constants.hpp>
#include <service-audio/AudioMessage.hpp>
#include <cassert>
extern "C"
{
#include "btstack_audio.h"
#include "btstack_debug.h"
#include "btstack_ring_buffer.h"
#include "classic/btstack_cvsd_plc.h"
#include "classic/btstack_sbc.h"
#include "classic/hfp.h"
#include "classic/hfp_msbc.h"
#include "hci.h"
}

namespace bluetooth
{

    class SCO::SCOImpl
    {
      public:
        static void init();
        static void send(hci_con_handle_t scoHandle);
        static void receive(uint8_t *packet, uint16_t size);
        void setOwnerService(const sys::Service *service);
        void setCodec(uint8_t codec);

      private:
        static constexpr auto BYTES_PER_FRAME     = 2;
        static constexpr auto ALL_GOOD_MASK       = 0x30;
        static constexpr auto AUDIO_BUFFER_LENGTH = 128;
        static constexpr auto PACKET_DATA_OFFSET  = 3;
        static constexpr auto VOICE_SETTING_CVSD  = 0x60; // linear, unsigned, 16-bit, CVSD

        static btstack_cvsd_plc_state_t cvsdPlcState;

        static QueueHandle_t sinkQueue;
        static QueueHandle_t sourceQueue;
        static DeviceMetadata_t metadata;
        static const sys::Service *ownerService;
        static uint8_t negotiated_codec;

        static auto audioInitialize(int sampleRate) -> Error;
        static void initCvsd();
        static void receiveCvsd(uint8_t *packet, uint16_t size);
        static void writeToHostEndian(int16_t *buffer, uint8_t *packet, int length);
        static void sendEvent(audio::EventType event, audio::Event::DeviceState state);
        static void flipEndianess(uint16_t *data, size_t length);
    };

    SCO::SCO() : pimpl(std::make_unique<SCOImpl>(SCOImpl()))
    {}

    SCO::SCO(SCO &&other) noexcept : pimpl(std::move(other.pimpl))
    {}
    auto SCO::operator=(SCO &&other) noexcept -> SCO &
    {
        if (&other == this) {
            return *this;
        }

        pimpl       = std::move(other.pimpl);
        other.pimpl = nullptr;
        return *this;
    }

    void SCO::init()
    {
        pimpl->init();
    }
    void SCO::send(hci_con_handle_t sco_handle)
    {
        pimpl->send(sco_handle);
    }
    void SCO::receive(uint8_t *packet, uint16_t size)
    {
        pimpl->receive(packet, size);
    }
    void SCO::setOwnerService(const sys::Service *service)
    {
        pimpl->setOwnerService(service);
    }
    void SCO::setCodec(SCOCodec codec)
    {
        pimpl->setCodec(static_cast<uint8_t>(codec));
    }

    SCO::~SCO() = default;
} // namespace bluetooth

using namespace bluetooth;

btstack_cvsd_plc_state_t SCO::SCOImpl::cvsdPlcState;
QueueHandle_t SCO::SCOImpl::sinkQueue;
QueueHandle_t SCO::SCOImpl::sourceQueue;
const sys::Service *SCO::SCOImpl::ownerService = nullptr;
DeviceMetadata_t SCO::SCOImpl::metadata;
uint8_t SCO::SCOImpl::negotiated_codec;

void SCO::SCOImpl::sendEvent(audio::EventType event, audio::Event::DeviceState state)
{
    auto evt       = std::make_shared<audio::Event>(event, state);
    auto msg       = std::make_shared<AudioEventRequest>(std::move(evt));
    auto &busProxy = const_cast<sys::Service *>(ownerService)->bus;
    busProxy.sendUnicast(std::move(msg), service::name::evt_manager);
}
auto SCO::SCOImpl::audioInitialize(int sampleRate) -> Error
{
    sourceQueue = xQueueCreate(5, sizeof(AudioData_t));
    sinkQueue   = xQueueCreate(5, sizeof(AudioData_t));

    int scoPayloadLength            = hci_get_sco_packet_length() - PACKET_DATA_OFFSET;
    const int audioSamplesPerPacket = scoPayloadLength;

    metadata.sampleRate      = static_cast<unsigned int>(sampleRate);
    metadata.channels        = 1;
    metadata.samplesPerFrame = audioSamplesPerPacket;

    if (sourceQueue == nullptr || sinkQueue == nullptr) {
        LOG_ERROR("failed to create queue!");
        return Error(Error::SystemError);
    }

    LOG_INFO("Init done!");
    return Error(Error::Success);
}

void SCO::SCOImpl::initCvsd()
{
    btstack_cvsd_plc_init(&cvsdPlcState);
    auto ret = audioInitialize(CVSD_SAMPLE_RATE);
    if (ret.err == Error::Success) {
        LOG_INFO("CVSD init done!");
    }
}
void SCO::SCOImpl::writeToHostEndian(int16_t *buffer, uint8_t *packet, int length)
{
    for (int i = 0; i < length; i++) {
        buffer[i] = little_endian_read_16(packet, PACKET_DATA_OFFSET + i * 2);
    }
}
void SCO::SCOImpl::receiveCvsd(uint8_t *packet, uint16_t size)
{

    std::array<int16_t, AUDIO_BUFFER_LENGTH> audioFrameOut{};

    if (size > audioFrameOut.size()) {
        LOG_WARN("SCO packet larger than local output buffer - dropping data.");
        return;
    }

    const int audioBytesRead = size - PACKET_DATA_OFFSET;
    const int numSamples     = audioBytesRead / BYTES_PER_FRAME;

    std::array<int16_t, AUDIO_BUFFER_LENGTH> audioFrameIn;
    writeToHostEndian(audioFrameIn.data(), packet, numSamples);

    // treat packet as bad frame if controller does not report 'all good'
    bool badFrame         = false;
    auto packetStatusByte = packet[1];
    if ((packetStatusByte & ALL_GOOD_MASK) != 0) {
        badFrame = true;
    }

    btstack_cvsd_plc_process_data(&cvsdPlcState, badFrame, audioFrameIn.data(), numSamples, audioFrameOut.data());

    // Samples in CVSD SCO packet are in little endian
    AudioData_t audioData;
    std::copy_n(std::begin(audioFrameOut), audioBytesRead, std::begin(audioData.data));
    audioData.bytesSent = audioBytesRead;

    if (sinkQueue != nullptr) {
        xQueueSend(sinkQueue, &audioData, 5);
    }
    else {
        LOG_ERROR("queue is nullptr!");
    }
}

void SCO::SCOImpl::init()
{
    hci_set_sco_voice_setting(VOICE_SETTING_CVSD);
    initCvsd();
}

void SCO::SCOImpl::flipEndianess(uint16_t *data, size_t length)
{
    for (size_t i = 0; i < length; i++) {
        data[i] = __builtin_bswap16(data[i]);
    }
}
void SCO::SCOImpl::send(hci_con_handle_t scoHandle)
{
    if (scoHandle == HCI_CON_HANDLE_INVALID) {
        return;
    }
    assert(sourceQueue != nullptr);

    int scoPacketLength  = hci_get_sco_packet_length();
    int scoPayloadLength = scoPacketLength - PACKET_DATA_OFFSET;

    hci_reserve_packet_buffer();
    auto scoPacket  = hci_get_outgoing_packet_buffer();
    auto sampleData = &scoPacket[3];
    AudioData_t audioData;

    if (xQueueReceive(sourceQueue, &audioData, 1) != pdPASS) {
        auto rangeStart = static_cast<uint8_t *>(sampleData);
        auto rangeEnd   = rangeStart + scoPayloadLength;
        std::fill(rangeStart, rangeEnd, 0);
    }
    else {
        auto dest = static_cast<std::uint8_t *>(sampleData);
        std::copy(std::begin(audioData.data), std::begin(audioData.data) + scoPayloadLength, dest);

        if (btstack_is_big_endian()) {
            flipEndianess(reinterpret_cast<uint16_t *>(sampleData), scoPayloadLength / 2);
        }
    }

    // set handle + flags
    little_endian_store_16(scoPacket, 0, scoHandle);
    // set length
    scoPacket[2] = scoPayloadLength;
    // finally send packet
    hci_send_sco_packet_buffer(scoPacketLength);
    // request another send event
    hci_request_sco_can_send_now_event();
}

void SCO::SCOImpl::receive(uint8_t *packet, uint16_t size)
{
    receiveCvsd(packet, size);
}
void SCO::SCOImpl::setOwnerService(const sys::Service *service)
{
    ownerService = service;
}

void SCO::SCOImpl::setCodec(uint8_t codec)
{
    if (negotiated_codec == codec) {
        return;
    }
    negotiated_codec = codec;

    if (negotiated_codec == HFP_CODEC_MSBC) {
        // btstack_sbc_decoder_init(&decoder_state, SBC_MODE_mSBC, &handle_pcm_data, NULL);
        hfp_msbc_init();
    }
    else {
        initCvsd();
    }
}
