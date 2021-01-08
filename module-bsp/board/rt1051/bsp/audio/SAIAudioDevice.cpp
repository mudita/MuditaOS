#include "SAIAudioDevice.hpp"

using namespace bsp;

SAIAudioDevice::SAIAudioDevice(AudioDevice::audioCallback_t callback,
                               I2S_Type *base,
                               sai_edma_handle_t *rxHandle,
                               sai_edma_handle_t *txHandle)
    : AudioDevice(callback), _base(base), rx(rxHandle), tx(txHandle)
{}

void SAIAudioDevice::initiateRxTransfer()
{
    audio::Stream::Span dataSpan;

    Source::_stream->reserve(dataSpan);
    auto xfer = sai_transfer_t{.data = dataSpan.data, .dataSize = dataSpan.dataSize};
    SAI_TransferReceiveEDMA(_base, rx, &xfer);
}

void SAIAudioDevice::initiateTxTransfer()
{
    auto nullSpan = Sink::_stream->getNullSpan();
    auto xfer     = sai_transfer_t{.data = nullSpan.data, .dataSize = nullSpan.dataSize};
    SAI_TransferSendEDMA(_base, tx, &xfer);
}

void SAIAudioDevice::onDataSend()
{
    audio::Stream::Span dataSpan;

    /// pop previous read and peek next
    Sink::_stream->consume();
    Sink::_stream->peek(dataSpan);

    sai_transfer_t xfer{.data = dataSpan.data, .dataSize = dataSpan.dataSize};
    SAI_TransferSendEDMA(_base, tx, &xfer);
}

void SAIAudioDevice::onDataReceive()
{
    audio::Stream::Span dataSpan;

    /// reserve space for the next read commiting previously reserved block before
    Source::_stream->commit();
    Source::_stream->reserve(dataSpan);

    sai_transfer_t xfer{.data = dataSpan.data, .dataSize = dataSpan.dataSize};
    SAI_TransferReceiveEDMA(_base, rx, &xfer);
}

void SAIAudioDevice::enableInput()
{}

void SAIAudioDevice::enableOutput()
{}

void SAIAudioDevice::disableInput()
{}

void SAIAudioDevice::disableOutput()
{}
