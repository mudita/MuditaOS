// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TestStream.hpp"

#include <Audio/AbstractStream.hpp>

#include <memory>
#include <utility>

using ::audio::AbstractStream;
using testing::audio::TestStream;

TestStream::TestStream(std::size_t blockSize) : bufSize(blockSize)
{
    data = std::make_unique<std::uint8_t[]>(bufSize);
}

const AbstractStream::Span TestStream::getDataSpan() const
{
    return AbstractStream::Span{data.get(), bufSize};
}

bool TestStream::push(void *data, std::size_t dataSize)
{
    return true;
}

bool TestStream::push(const AbstractStream::Span &span)
{
    if (span.dataSize != bufSize) {
        return false;
    }

    for (std::size_t i = 0; i < span.dataSize; i++) {
        data.get()[i] = span.data[i];
    }

    return true;
}

bool TestStream::push()
{
    return true;
}

bool TestStream::pop(AbstractStream::Span &span)
{
    return true;
}

bool TestStream::reserve(AbstractStream::Span &span)
{
    span = Span{.data = data.get(), .dataSize = bufSize};
    return true;
}

void TestStream::commit()
{}

void TestStream::release()
{}

bool TestStream::peek(AbstractStream::Span &span)
{
    return true;
}

void TestStream::consume()
{}

void TestStream::unpeek()
{}

void TestStream::reset()
{}

void TestStream::setData(uint8_t value)
{
    for (std::size_t i = 0; i < bufSize; i++) {
        data.get()[i] = value;
    }
}

bool TestStream::checkData(const Span &span)
{
    if (span.dataSize != bufSize) {
        return false;
    }

    for (std::size_t i = 0; i < bufSize; i++) {
        if (data.get()[i] != span.data[i]) {
            return false;
        }
    }

    return true;
}

void TestStream::registerListener(EventListener *listener)
{}

void TestStream::unregisterListeners(EventListener *listener)
{}

auto TestStream::getInputTraits() const noexcept -> Traits
{
    return Traits{.blockSize = bufSize, .format = ::audio::nullFormat};
}

auto TestStream::getOutputTraits() const noexcept -> Traits
{
    return Traits{.blockSize = bufSize, .format = ::audio::nullFormat};
}

bool TestStream::isEmpty() const noexcept
{
    return false;
}

bool TestStream::isFull() const noexcept
{
    return false;
}
