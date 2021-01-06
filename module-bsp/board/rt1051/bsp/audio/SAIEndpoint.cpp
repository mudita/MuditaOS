#include "SAIEndpoint.hpp"

using namespace bsp;

SAIEndpoint::SAIEndpoint(sai_edma_handle_t *handle, I2S_Type *base) : _handle(handle), _base(base)
{}

void SAIEndpoint::start()
{}

void SAIEndpoint::stop()
{}

SAISource::SAISource(sai_edma_handle_t *handle, I2S_Type *base) : SAIEndpoint(handle, base)
{}

SAISink::SAISink(sai_edma_handle_t *handle, I2S_Type *base) : SAIEndpoint(handle, base)
{}

void SAISource::dataWrite()
{
    audio::Stream::Span dataSpan;

    /// reserve space for the next read commiting previously reserved block before
    _stream->commit();
    _stream->reserve(dataSpan);

    sai_transfer_t xfer{.data = dataSpan.data, .dataSize = dataSpan.dataSize};
    SAI_TransferReceiveEDMA(_base, _handle, &xfer);
}

void SAISource::initiateSAITransfer()
{
    audio::Stream::Span dataSpan;
    _stream->reserve(dataSpan);
    auto xfer = sai_transfer_t{.data = dataSpan.data, .dataSize = dataSpan.dataSize};
    SAI_TransferReceiveEDMA(_base, _handle, &xfer);
}

void SAISink::dataRead()
{
    audio::Stream::Span dataSpan;

    /// pop previous read and peek next
    _stream->consume();
    _stream->peek(dataSpan);

    sai_transfer_t xfer{.data = dataSpan.data, .dataSize = dataSpan.dataSize};
    SAI_TransferSendEDMA(_base, _handle, &xfer);
}

void SAISink::initiateSAITransfer()
{
    auto nullSpan = _stream->getNullSpan();
    auto xfer     = sai_transfer_t{.data = nullSpan.data, .dataSize = nullSpan.dataSize};
    SAI_TransferSendEDMA(_base, _handle, &xfer);
}

void SAISource::start()
{
    SAIEndpoint::start();
}

void SAISource::stop()
{
    SAIEndpoint::stop();
}

void SAISink::start()
{
    SAIEndpoint::start();
}

void SAISink::stop()
{
    SAIEndpoint::stop();
}
