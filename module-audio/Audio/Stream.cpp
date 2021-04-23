// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Stream.hpp"

#include <algorithm>
#include <iterator>

using namespace audio;

Stream::Stream(AudioFormat format, Allocator &allocator, std::size_t blockSize, unsigned int bufferingSize)
    : _allocator(allocator), _blockSize(blockSize), _blockCount(bufferingSize), _format(format),
      _buffer(_allocator.allocate(_blockSize * _blockCount)), _emptyBuffer(_allocator.allocate(_blockSize)),
      _dataStart(_buffer.get(), _blockSize * _blockCount, _buffer.get(), _blockSize), _dataEnd(_dataStart),
      _peekPosition(_dataStart), _writeReservationPosition(_dataStart)
{
    std::fill(_emptyBuffer.get(), _emptyBuffer.get() + blockSize, 0);
}

bool Stream::push(void *data, std::size_t dataSize)
{
    /// wrapper - no synchronization needed
    return push(Span{.data = static_cast<std::uint8_t *>(data), .dataSize = dataSize});
}

bool Stream::push(const Span &span)
{
    LockGuard lock;

    /// sanity - do not store buffers different than internal block size
    if (span.dataSize != _blockSize) {
        return false;
    }

    /// write reservation in progress
    if (_dataEnd != _writeReservationPosition) {
        return false;
    }

    /// no space left
    if (isFull()) {
        broadcastEvent(Event::StreamOverflow);
        return false;
    }

    auto nextDataBlock = *_dataEnd;
    std::copy(span.data, span.dataEnd(), nextDataBlock.data);

    _dataEnd++;
    _blocksUsed++;
    _writeReservationPosition = _dataEnd;

    broadcastStateEvents();

    return true;
}

bool Stream::push()
{
    /// wrapper - no synchronization needed
    return push(getNullSpan());
}

bool Stream::pop(Span &span)
{
    LockGuard lock;

    /// sanity - do not store buffers different than internal block size
    if (span.dataSize != _blockSize) {
        return false;
    }

    /// peek in progress
    if (_dataStart != _peekPosition) {
        return false;
    }

    if (isEmpty()) {
        span = getNullSpan();
        broadcastEvent(Event::StreamUnderFlow);
        return false;
    }

    std::copy((*_dataStart).data, (*_dataStart).dataEnd(), span.data);

    _dataStart++;
    _blocksUsed--;
    _peekPosition = _dataStart;

    broadcastStateEvents();
    return true;
}

void Stream::consume()
{
    LockGuard lock;

    _blocksUsed -= _peekCount;
    _peekCount = 0;
    _dataStart = _peekPosition;

    broadcastStateEvents();
}

bool Stream::peek(Span &span)
{
    LockGuard lock;

    if (getPeekedCount() < getUsedBlockCount()) {
        span = *_peekPosition++;
        _peekCount++;
        return true;
    }

    span = getNullSpan();
    broadcastEvent(Event::StreamUnderFlow);
    return false;
}

void Stream::unpeek()
{
    LockGuard lock;

    _peekPosition = _dataStart;
    _peekCount    = 0;
}

bool Stream::reserve(Span &span)
{
    LockGuard lock;

    if (getBlockCount() - getUsedBlockCount() > _reserveCount) {
        span = *++_writeReservationPosition;
        _reserveCount++;
        return true;
    }

    // reset data to peek end
    _blocksUsed = 0;
    _dataEnd    = _peekPosition;

    // reserve at peek end
    _reserveCount             = 1;
    _writeReservationPosition = _peekPosition;
    span                      = *++_writeReservationPosition;

    broadcastEvent(Event::StreamOverflow);
    return false;
}

void Stream::commit()
{
    LockGuard lock;

    _blocksUsed += _reserveCount;
    _reserveCount = 0;
    _dataEnd      = _writeReservationPosition;

    broadcastStateEvents();
}

void Stream::release()
{
    LockGuard lock;

    _reserveCount             = 0;
    _writeReservationPosition = _dataEnd;
}

auto Stream::getInputTraits() const noexcept -> Traits
{
    LockGuard lock;
    return getIOTraits();
}

auto Stream::getOutputTraits() const noexcept -> Traits
{
    LockGuard lock;
    return getIOTraits();
}

auto Stream::getIOTraits() const noexcept -> Traits
{
    return Traits{.blockSize = _blockSize, .format = _format};
}

void Stream::registerListener(AbstractStream::EventListener *listener)
{
    LockGuard lock;

    listeners.push_back(std::ref(listener));
}

void Stream::unregisterListeners(AbstractStream::EventListener *listener)
{
    LockGuard lock;

    auto it = std::find(listeners.begin(), listeners.end(), listener);
    if (it != listeners.end()) {
        listeners.erase(it);
    }
}

void Stream::broadcastEvent(Event event)
{
    for (auto listener : listeners) {
        listener->onEvent(this, event);
    }
}

void Stream::broadcastStateEvents()
{
    if (_blocksUsed == (getBlockCount() / 2)) {
        broadcastEvent(Event::StreamHalfUsed);
    }

    else if (isEmpty()) {
        broadcastEvent(Event::StreamEmpty);
    }

    else if (isFull()) {
        broadcastEvent(Event::StreamFull);
    }
}

std::size_t Stream::getBlockCount() const noexcept
{
    return _blockCount;
}

std::size_t Stream::getUsedBlockCount() const noexcept
{
    return _blocksUsed;
}

std::size_t Stream::getPeekedCount() const noexcept
{
    return _peekCount;
}

std::size_t Stream::getReservedCount() const noexcept
{
    return _reserveCount;
}

bool Stream::isEmpty() const noexcept
{
    LockGuard lock;
    return getUsedBlockCount() == 0;
}

bool Stream::isFull() const noexcept
{
    LockGuard lock;
    return getUsedBlockCount() == getBlockCount();
}

bool Stream::blocksAvailable() const noexcept
{
    return !isFull();
}

void Stream::reset()
{
    LockGuard lock;

    _dataStart                = {_buffer.get(), _blockSize * _blockCount, _buffer.get(), _blockSize};
    _dataEnd                  = _dataStart;
    _peekPosition             = _dataStart;
    _writeReservationPosition = _dataStart;
    std::fill(_emptyBuffer.get(), _emptyBuffer.get() + _blockSize, 0);

    _blocksUsed   = 0;
    _peekCount    = 0;
    _reserveCount = 0;
}

Stream::UniqueStreamBuffer StandardStreamAllocator::allocate(std::size_t size)
{
    return std::make_unique<uint8_t[]>(size);
}

Stream::UniqueStreamBuffer NonCacheableStreamAllocator::allocate(std::size_t size)
{
    return std::unique_ptr<std::uint8_t[], std::function<void(uint8_t[])>>(
        allocator.allocate(size), [this, size](std::uint8_t ptr[]) { allocator.deallocate(ptr, size); });
}

Stream::RawBlockIterator::RawBlockIterator(std::uint8_t *bufStart,
                                           std::size_t bufSize,
                                           std::uint8_t *ptr,
                                           std::size_t stepSize)
    : _bufStart(bufStart), _bufEnd(bufStart + bufSize), _curPos(ptr), _stepSize(stepSize)
{}

Stream::RawBlockIterator &Stream::RawBlockIterator::operator++()
{
    _curPos += _stepSize;
    if (_curPos == _bufEnd) {
        _curPos = _bufStart;
    }

    return *this;
}

bool Stream::RawBlockIterator::operator==(const Stream::RawBlockIterator &rhs)
{
    return _curPos == rhs._curPos;
}

bool Stream::RawBlockIterator::operator!=(const Stream::RawBlockIterator &rhs)
{
    return !operator==(rhs);
}

Stream::RawBlockIterator Stream::RawBlockIterator::operator++(int)
{
    RawBlockIterator tmp(*this);
    operator++();
    return tmp;
}

Stream::RawBlockIterator &Stream::RawBlockIterator::operator--()
{
    if (_curPos == _bufStart) {
        _curPos = _bufEnd - _stepSize;
    }
    else {
        _curPos -= _stepSize;
    }

    return *this;
}

Stream::RawBlockIterator Stream::RawBlockIterator::operator--(int)
{
    RawBlockIterator tmp(*this);
    operator--();
    return tmp;
}

Stream::Span Stream::RawBlockIterator::operator*()
{
    return Stream::Span{.data = _curPos, .dataSize = _stepSize};
}

Stream::Span Stream::getNullSpan() const noexcept
{
    return Span{.data = _emptyBuffer.get(), .dataSize = _blockSize};
}
