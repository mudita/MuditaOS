// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothAudioDevice.hpp"

#include <interface/profiles/A2DP/AVDTP.hpp>
#include <interface/profiles/A2DP/AVRCP.hpp>
#include <interface/profiles/HSP/SCO.hpp>

#include <Audio/AudioCommon.hpp>
#include <Audio/VolumeScaler.hpp>
#include <Audio/Stream.hpp>

#include <chrono>
#include <cassert>

using audio::AudioFormat;
using bluetooth::A2DPAudioDevice;
using bluetooth::BluetoothAudioDevice;
using bluetooth::HSPAudioDevice;

using namespace std::chrono_literals;

BluetoothAudioDevice::BluetoothAudioDevice(AudioProfile audioProfile) : profile(audioProfile)
{}

BluetoothAudioDevice::~BluetoothAudioDevice()
{
    LOG_DEBUG("Destroying bluetooth audio device");
}

auto BluetoothAudioDevice::getProfileType() const -> AudioProfile
{
    return profile;
}

auto BluetoothAudioDevice::setOutputVolume(float vol) -> audio::AudioDevice::RetCode
{
    const auto volumeToSet = audio::volume::scaler::toAvrcpVolume(vol);
    const auto status      = avrcp_controller_set_absolute_volume(AVRCP::mediaTracker.avrcp_cid, volumeToSet);
    if (status != ERROR_CODE_SUCCESS) {
        LOG_ERROR("Can't set volume level. Status %x", status);
        return audio::AudioDevice::RetCode::Failure;
    }

    outputVolume = vol;
    return audio::AudioDevice::RetCode::Success;
}

auto BluetoothAudioDevice::setInputGain(float gain) -> audio::AudioDevice::RetCode
{
    return audio::AudioDevice::RetCode::Success;
}

auto BluetoothAudioDevice::isInputEnabled() const -> bool
{
    return inputEnabled;
}

auto BluetoothAudioDevice::isOutputEnabled() const -> bool
{
    return outputEnabled;
}

void A2DPAudioDevice::onDataSend()
{
    if (isOutputEnabled()) {
        fillSbcAudioBuffer();
    }
}

void A2DPAudioDevice::onDataReceive()
{}

void HSPAudioDevice::onDataSend()
{}

void HSPAudioDevice::onDataSend(std::uint16_t scoHandle)
{
    if (!isOutputEnabled()) {
        return;
    }

    hci_reserve_packet_buffer();
    auto scoPacket = hci_get_outgoing_packet_buffer();

    // get data to send
    audio::AbstractStream::Span dataSpan;
    Sink::_stream->peek(dataSpan);

    assert(dataSpan.dataSize <= scoPayloadLength);

    // prepare packet to send
    std::copy(dataSpan.data, dataSpan.dataEnd(), &scoPacket[packetDataOffset]);
    Sink::_stream->consume();
    little_endian_store_16(scoPacket, packetHandleOffset, scoHandle);
    scoPacket[packetLengthOffset] = dataSpan.dataSize;

    // send packet
    hci_send_sco_packet_buffer(dataSpan.dataSize + packetDataOffset);
    hci_request_sco_can_send_now_event();
}

void HSPAudioDevice::receiveCVSD(audio::AbstractStream::Span receivedData)
{
    auto blockSize          = Source::_stream->getInputTraits().blockSize;
    auto processedDataIndex = 0;

    // TODO: decode CVSD

    // try to complete leftovers to the full block size
    if (leftoversSize != 0) {
        auto maxFillSize = blockSize - leftoversSize;
        auto fillSize    = std::min(maxFillSize, receivedData.dataSize);

        std::copy_n(receivedData.data, fillSize, &rxLeftovers[leftoversSize]);

        if (fillSize + leftoversSize < blockSize) {
            leftoversSize += fillSize;
            return;
        }

        Source::_stream->push(&rxLeftovers[0], blockSize);
        leftoversSize      = 0;
        processedDataIndex = fillSize;
    }

    // push as many blocks as possible
    while (receivedData.dataSize - processedDataIndex >= blockSize) {
        Source::_stream->push(&receivedData.data[processedDataIndex], blockSize);
        processedDataIndex += blockSize;
    }

    // save leftovers
    leftoversSize = receivedData.dataSize - processedDataIndex;
    if (leftoversSize > 0) {
        std::copy_n(&receivedData.data[processedDataIndex], leftoversSize, &rxLeftovers[0]);
    }
}

void HSPAudioDevice::onDataReceive()
{}

void BluetoothAudioDevice::enableInput()
{
    inputEnabled = true;
}

void BluetoothAudioDevice::enableOutput()
{
    LOG_DEBUG("Enabling bluetooth audio output.");
    outputEnabled = true;
}

void BluetoothAudioDevice::disableInput()
{
    inputEnabled = false;
}

void BluetoothAudioDevice::disableOutput()
{
    LOG_DEBUG("Disabling bluetooth audio output.");
    outputEnabled = false;
}

void HSPAudioDevice::enableInput()
{
    auto blockSize = Source::_stream->getInputTraits().blockSize;
    rxLeftovers    = std::make_unique<std::uint8_t[]>(blockSize);
    BluetoothAudioDevice::enableInput();
}

auto BluetoothAudioDevice::fillSbcAudioBuffer() -> int
{
    // perform sbc encodin
    int totalNumBytesRead                    = 0;
    unsigned int numAudioSamplesPerSbcBuffer = btstack_sbc_encoder_num_audio_frames();
    auto context                             = &AVRCP::mediaTracker;

    assert(context != nullptr);

    while (context->samples_ready >= numAudioSamplesPerSbcBuffer &&
           (context->max_media_payload_size - context->sbc_storage_count) >= btstack_sbc_encoder_sbc_buffer_length()) {
        audio::Stream::Span dataSpan;

        Sink::_stream->peek(dataSpan);
        btstack_sbc_encoder_process_data(reinterpret_cast<int16_t *>(dataSpan.data));
        Sink::_stream->consume();

        uint16_t sbcFrameSize = btstack_sbc_encoder_sbc_buffer_length();
        uint8_t *sbcFrame     = btstack_sbc_encoder_sbc_buffer();

        totalNumBytesRead += numAudioSamplesPerSbcBuffer;
        memcpy(&context->sbc_storage[context->sbc_storage_count], sbcFrame, sbcFrameSize);
        context->sbc_storage_count += sbcFrameSize;
        context->samples_ready -= numAudioSamplesPerSbcBuffer;
    }

    return totalNumBytesRead;
}

auto A2DPAudioDevice::getSupportedFormats() -> std::vector<audio::AudioFormat>
{
    constexpr static auto supportedBitWidth = 16U;
    return std::vector<AudioFormat>{AudioFormat{static_cast<unsigned>(AVDTP::sbcConfig.samplingFrequency),
                                                supportedBitWidth,
                                                static_cast<unsigned>(AVDTP::sbcConfig.numChannels)}};
}

auto HSPAudioDevice::getSupportedFormats() -> std::vector<audio::AudioFormat>
{
    return std::vector<AudioFormat>{getSourceFormat()};
}

auto A2DPAudioDevice::getTraits() const -> ::audio::Endpoint::Traits
{
    return Traits{.usesDMA = false, .blockSizeConstraint = 512U, .timeConstraint = 10ms};
}

auto HSPAudioDevice::getTraits() const -> ::audio::Endpoint::Traits
{
    return Traits{.usesDMA = false, .blockSizeConstraint = 32U, .timeConstraint = 16ms};
}

auto A2DPAudioDevice::getSourceFormat() -> ::audio::AudioFormat
{
    return audio::nullFormat;
}

auto HSPAudioDevice::getSourceFormat() -> ::audio::AudioFormat
{
    constexpr static auto supportedBitWidth = 16U;
    constexpr static auto supportedChannels = 1;
    return AudioFormat{bluetooth::SCO::CVSD_SAMPLE_RATE, supportedBitWidth, supportedChannels};
}
