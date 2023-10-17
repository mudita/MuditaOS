// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SAIAudioDevice.hpp"

#include <Audio/Stream.hpp>
#include <log/log.hpp>

#include <cmath>

using namespace audio;

SAIAudioDevice::SAIAudioDevice(I2S_Type *base, sai_edma_handle_t *rxHandle, sai_edma_handle_t *txHandle)
    : _base(base), rx(rxHandle), tx(txHandle)
{}

void SAIAudioDevice::initiateRxTransfer()
{
    audio::Stream::Span dataSpan;

    Source::_stream->reserve(dataSpan);
    LOG_DEBUG("Initiate rx transfer with %u bytes", dataSpan.dataSize);
    auto xfer = sai_transfer_t{.data = dataSpan.data, .dataSize = dataSpan.dataSize};
    SAI_TransferReceiveEDMA(_base, rx, &xfer);
}

void SAIAudioDevice::initiateTxTransfer()
{
    audio::Stream::Span dataToSend;
    Sink::_stream->peek(dataToSend);

    auto xfer = sai_transfer_t{.data = dataToSend.data, .dataSize = dataToSend.dataSize};
    SAI_TransferSendEDMA(_base, tx, &xfer);
}

void SAIAudioDevice::onDataSend()
{
    audio::Stream::Span dataSpan;

    if (!txEnabled || !isSinkConnected()) {
        return;
    }

    /// pop previous read and peek next
    Sink::_stream->consume();
    Sink::_stream->peek(dataSpan);
    scaleOutputVolume(dataSpan);

    sai_transfer_t xfer{.data = dataSpan.data, .dataSize = dataSpan.dataSize};
    SAI_TransferSendEDMA(_base, tx, &xfer);
}

void SAIAudioDevice::onDataReceive()
{
    audio::Stream::Span dataSpan;

    if (!rxEnabled || !isSourceConnected()) {
        return;
    }

    /// reserve space for the next read commiting previously reserved block before
    Source::_stream->commit();
    Source::_stream->reserve(dataSpan);

    sai_transfer_t xfer{.data = dataSpan.data, .dataSize = dataSpan.dataSize};
    SAI_TransferReceiveEDMA(_base, rx, &xfer);
}

void SAIAudioDevice::enableInput()
{
    if (!isSourceConnected()) {
        LOG_FATAL("No output stream connected!");
        return;
    }

    rxEnabled = true;

    /// initiate first read
    initiateRxTransfer();
}

void SAIAudioDevice::enableOutput()
{
    if (!isSinkConnected()) {
        LOG_FATAL("No input stream connected!");
        return;
    }

    txEnabled = true;

    /// initiate first write
    initiateTxTransfer();
}

void SAIAudioDevice::disableInput()
{
    rxEnabled = false;
}

void SAIAudioDevice::disableOutput()
{
    txEnabled = false;
}
AudioDevice::RetCode SAIAudioDevice::setOutputVolume(float vol)
{
    vol = std::clamp(vol, minVolume, maxVolume);
    /// Using y=x^2 function as an approximation seems very natural and has the most useful range
    /// For more info check: https://www.dr-lex.be/info-stuff/volumecontrols.html
    volumeFactor = std::pow(1.0f * (vol / maxVolume), 2);
    return AudioDevice::RetCode::Success;
}
void SAIAudioDevice::scaleOutputVolume(audio::Stream::Span &span)
{
    if (volumeFactor < 1.0) {
        const auto samples      = reinterpret_cast<std::int16_t *>(span.data);
        const auto samplesCount = samples + span.dataSize / 2;
        std::for_each(samples, samplesCount, [this](auto &sample) { sample *= volumeFactor; });
    }
}
