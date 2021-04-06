// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothAudioDevice.hpp"

#include <Audio/AudioCommon.hpp>
#include <interface/profiles/A2DP/AVDTP.hpp>

#include <Audio/Stream.hpp>

#include <cassert>

using audio::AudioFormat;
using namespace bluetooth;

BluetoothAudioDevice::BluetoothAudioDevice(MediaContext *mediaContext)
    : AudioDevice(btCapabilities, btCapabilities), ctx(mediaContext)
{
    LOG_DEBUG("Bluetooth audio device created");
}

BluetoothAudioDevice::~BluetoothAudioDevice()
{
    LOG_DEBUG("Destroying bluetooth audio device");
}

void BluetoothAudioDevice::setMediaContext(MediaContext *mediaContext)
{
    constexpr static auto supportedBitWidth = 16U;
    ctx                                     = mediaContext;
    formats = std::vector<AudioFormat>{AudioFormat{static_cast<unsigned>(AVDTP::sbcConfig.samplingFrequency),
                                                   supportedBitWidth,
                                                   static_cast<unsigned>(AVDTP::sbcConfig.numChannels)}};
}

auto BluetoothAudioDevice::Start(const Format &format) -> audio::AudioDevice::RetCode
{
    return audio::AudioDevice::RetCode::Success;
}

auto BluetoothAudioDevice::Stop() -> audio::AudioDevice::RetCode
{
    return audio::AudioDevice::RetCode::Success;
}

auto BluetoothAudioDevice::OutputVolumeCtrl(float vol) -> audio::AudioDevice::RetCode
{
    constexpr auto avrcpMaxVolume = std::uint8_t{0x7F}; // from AVRCP documentation
    const auto volumeToSet        = static_cast<std::uint8_t>((vol / audio::maxVolume) * avrcpMaxVolume);
    const auto status             = avrcp_controller_set_absolute_volume(ctx->avrcp_cid, volumeToSet);
    if (status != ERROR_CODE_SUCCESS) {
        LOG_ERROR("Can't set volume level. Status %x", status);
        return audio::AudioDevice::RetCode::Failure;
    }
    currentFormat.outputVolume = vol;
    return audio::AudioDevice::RetCode::Success;
}

auto BluetoothAudioDevice::InputGainCtrl(float gain) -> audio::AudioDevice::RetCode
{
    return audio::AudioDevice::RetCode::Success;
}

auto BluetoothAudioDevice::OutputPathCtrl(OutputPath outputPath) -> audio::AudioDevice::RetCode
{
    return audio::AudioDevice::RetCode::Success;
}

auto BluetoothAudioDevice::InputPathCtrl(InputPath inputPath) -> audio::AudioDevice::RetCode
{
    return audio::AudioDevice::RetCode::Success;
}

auto BluetoothAudioDevice::IsFormatSupported(const Format &format) -> bool
{
    return true;
}

void BluetoothAudioDevice::onDataSend()
{
    if (outputEnabled) {
        fillSbcAudioBuffer(ctx);
    }
}

void BluetoothAudioDevice::onDataReceive()
{}

void BluetoothAudioDevice::enableInput()
{}

void BluetoothAudioDevice::enableOutput()
{
    LOG_DEBUG("Enabling bluetooth audio output.");
    outputEnabled = true;
}

void BluetoothAudioDevice::disableInput()
{}

void BluetoothAudioDevice::disableOutput()
{
    LOG_DEBUG("Disabling bluetooth audio output.");
    outputEnabled = false;
}

auto BluetoothAudioDevice::fillSbcAudioBuffer(MediaContext *context) -> int
{
    // perform sbc encodin
    int totalNumBytesRead                    = 0;
    unsigned int numAudioSamplesPerSbcBuffer = btstack_sbc_encoder_num_audio_frames();

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

auto BluetoothAudioDevice::getSupportedFormats() -> const std::vector<AudioFormat> &
{
    return formats;
}
