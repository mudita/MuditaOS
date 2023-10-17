// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "StreamProxy.hpp"
#include "AbstractStream.hpp"

using audio::StreamProxy;

StreamProxy::StreamProxy(std::shared_ptr<AbstractStream> wrappedStream) noexcept : wrappedStream(wrappedStream)
{}

bool StreamProxy::push(void *data, std::size_t dataSize)
{
    return wrappedStream->push(data, dataSize);
}

bool StreamProxy::push(const Span &span)
{
    return wrappedStream->push(span);
}

bool StreamProxy::push()
{
    return wrappedStream->push();
}

bool StreamProxy::pop(Span &span)
{
    return wrappedStream->pop(span);
}

bool StreamProxy::reserve(Span &span)
{
    return wrappedStream->reserve(span);
}

void StreamProxy::commit()
{
    wrappedStream->commit();
}

void StreamProxy::release()
{
    wrappedStream->release();
}

bool StreamProxy::peek(Span &span)
{
    return wrappedStream->peek(span);
}

void StreamProxy::consume()
{
    wrappedStream->consume();
}

void StreamProxy::unpeek()
{
    wrappedStream->unpeek();
}

void StreamProxy::reset()
{
    wrappedStream->reset();
}

void StreamProxy::registerListener(EventListener *listener)
{
    wrappedStream->registerListener(listener);
}

void StreamProxy::unregisterListeners(EventListener *listener)
{
    wrappedStream->unregisterListeners(listener);
}

auto StreamProxy::getOutputTraits() const noexcept -> Traits
{
    return wrappedStream->getOutputTraits();
}

auto StreamProxy::getInputTraits() const noexcept -> Traits
{
    return wrappedStream->getInputTraits();
}

bool StreamProxy::isEmpty() const noexcept
{
    return wrappedStream->isEmpty();
}

bool StreamProxy::isFull() const noexcept
{
    return wrappedStream->isFull();
}

auto StreamProxy::getWrappedStream() -> AbstractStream &
{
    return *wrappedStream;
}
