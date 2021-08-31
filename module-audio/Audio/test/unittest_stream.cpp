// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Audio/Stream.hpp>
#include <Audio/AudioFormat.hpp>
#include <Audio/StreamProxy.hpp>
#include <Audio/StreamFactory.hpp>
#include <Audio/transcode/BasicDecimator.hpp>

#include "MockEndpoint.hpp"
#include "MockStream.hpp"

#include <memory>

#include <cstdint>
#include <cstring>

using audio::NonCacheableStreamAllocator;
using audio::StandardStreamAllocator;
using audio::Stream;
using audio::StreamFactory;
using testing::Return;
using testing::audio::MockStream;
using testing::audio::MockStreamEventListener;

using namespace std::chrono_literals;

constexpr std::size_t defaultBlockSize = 64U;
constexpr std::size_t defaultBuffering = 32U;
constexpr audio::AudioFormat format    = audio::AudioFormat(44100, 16, 2);

static std::uint8_t testData[defaultBuffering][defaultBlockSize];
static std::uint8_t emptyBlock[defaultBlockSize];

static void initTestData()
{
    auto fillbuf = [](std::uint8_t *b, std::size_t s, unsigned step) {
        std::uint8_t v = 0;
        for (unsigned int i = 0; i < s; ++i, v += step) {
            b[i] = v; // & (UINT8_MAX - 1);
        }
    };

    fillbuf(testData[0], defaultBlockSize, 1);
    fillbuf(testData[1], defaultBlockSize, 3);
    fillbuf(testData[2], defaultBlockSize, 7);
    fillbuf(testData[3], defaultBlockSize, 13);
    fillbuf(emptyBlock, defaultBlockSize, 0);
}

static void printBuf(std::uint8_t *buf, std::size_t s)
{
    for (unsigned int i = 0; i < s; i++) {
        std::cout << static_cast<unsigned int>(buf[i]) << " ";
    }

    std::cout << std::endl;
}

[[maybe_unused]] static void printBuf(Stream::Span s)
{
    printBuf(s.data, s.dataSize);
}

TEST(Stream, Init)
{
    StandardStreamAllocator a;
    constexpr auto bufferingSize = 2U;
    Stream s(format, a, defaultBlockSize, bufferingSize);

    EXPECT_EQ(s.getBlockCount(), bufferingSize);
    EXPECT_EQ(s.getInputTraits().blockSize, defaultBlockSize);
    EXPECT_EQ(s.getOutputTraits().blockSize, defaultBlockSize);
    EXPECT_EQ(s.getUsedBlockCount(), 0);
}

TEST(Stream, DefaultBuffering)
{
    StandardStreamAllocator allocator;
    Stream stream(format, allocator, defaultBlockSize);
    EXPECT_EQ(stream.getBlockCount(), defaultBuffering);
}

TEST(Stream, EmptyFull)
{
    StandardStreamAllocator a;
    constexpr auto bufferingSize = 2U;
    Stream s(format, a, defaultBlockSize, bufferingSize);

    EXPECT_TRUE(s.isEmpty());
    EXPECT_TRUE(s.blocksAvailable());
    EXPECT_FALSE(s.isFull());

    s.push();

    EXPECT_FALSE(s.isEmpty());
    EXPECT_TRUE(s.blocksAvailable());
    EXPECT_FALSE(s.isFull());

    s.push();

    EXPECT_FALSE(s.isEmpty());
    EXPECT_FALSE(s.blocksAvailable());
    EXPECT_TRUE(s.isFull());
}

TEST(Stream, Allocator)
{
    NonCacheableStreamAllocator a;
    EXPECT_NO_THROW(Stream(format, a, defaultBlockSize));
}

TEST(Stream, Listeners)
{
    testing::audio::MockStreamEventListener listener;
    StandardStreamAllocator a;
    Stream s(format, a, defaultBlockSize);
    Stream::Span span;

    s.registerListener(&listener);

    // does not take effect
    s.unregisterListeners(nullptr);

    EXPECT_CALL(listener, onEvent(&s, ::audio::AbstractStream::Event::StreamUnderFlow)).Times(1);
    s.peek(span);

    s.unregisterListeners(&listener);

    // no second onEvent call - listener is unregistered
    s.peek(span);
}

TEST(Stream, Push)
{
    StandardStreamAllocator a;
    Stream s(format, a, defaultBlockSize);
    auto block = testData[0];

    for (std::size_t i = 0; i < defaultBuffering; i++) {
        EXPECT_TRUE(s.push(block, defaultBlockSize));
    }
    EXPECT_EQ(s.getUsedBlockCount(), defaultBuffering);
    EXPECT_FALSE(s.push(block, defaultBlockSize));
}

TEST(Stream, InvalidPushPop)
{
    StandardStreamAllocator a;
    std::uint8_t buf[defaultBlockSize];
    Stream s(format, a, defaultBlockSize);
    Stream::Span span{.data = buf, .dataSize = 63};

    EXPECT_FALSE(s.push(span));
    span.dataSize = defaultBlockSize;

    // push and peek null data
    EXPECT_TRUE(s.push());
    EXPECT_TRUE(s.peek(span));
    memset(buf, 0, sizeof(buf));

    ASSERT_EQ(span.dataSize, defaultBlockSize);
    EXPECT_EQ(memcmp(span.data, buf, span.dataSize), 0);

    // peek in progress - push is not allowed
    EXPECT_FALSE(s.pop(span));

    // push is not allowed when using zero copy write
    EXPECT_TRUE(s.reserve(span));
    EXPECT_FALSE(s.push(span));
}

TEST(Stream, PushPop)
{
    StandardStreamAllocator a;
    Stream s(format, a, defaultBlockSize);

    initTestData();

    EXPECT_TRUE(s.push(testData[0], defaultBlockSize));
    EXPECT_EQ(s.getUsedBlockCount(), 1);

    {
        std::uint8_t buf[defaultBlockSize];
        Stream::Span popped = {.data = buf, .dataSize = defaultBlockSize};
        EXPECT_TRUE(s.pop(popped));
        ASSERT_EQ(popped.dataSize, defaultBlockSize);
        ASSERT_EQ(popped.data, buf);
        ASSERT_EQ(memcmp(popped.data, testData[0], defaultBlockSize), 0);
    }

    {
        std::uint8_t buf[defaultBlockSize];
        Stream::Span popped = {.data = buf, .dataSize = defaultBlockSize};
        EXPECT_FALSE(s.pop(popped));
        ASSERT_EQ(popped.dataSize, defaultBlockSize);
        ASSERT_NE(popped.data, buf);
        ASSERT_EQ(memcmp(popped.data, emptyBlock, defaultBlockSize), 0);
    }

    {
        for (unsigned int i = 0; i < s.getBlockCount(); ++i) {
            ASSERT_TRUE(s.push(testData[i], defaultBlockSize));
        }

        for (unsigned int i = 0; i < s.getBlockCount(); ++i) {
            std::uint8_t buf[defaultBlockSize];
            Stream::Span popped = {.data = buf, .dataSize = defaultBlockSize};
            EXPECT_TRUE(s.pop(popped));
            ASSERT_EQ(popped.dataSize, defaultBlockSize);
            ASSERT_EQ(popped.data, buf);
            ASSERT_EQ(memcmp(popped.data, testData[i], defaultBlockSize), 0);
        }
    }

    ASSERT_EQ(s.getUsedBlockCount(), 0);
}

TEST(Stream, Peek)
{
    StandardStreamAllocator a;
    Stream s(format, a, defaultBlockSize);

    initTestData();

    {
        Stream::Span span;

        EXPECT_FALSE(s.peek(span));
        ASSERT_EQ(memcmp(span.data, emptyBlock, span.dataSize), 0);
    }

    {
        Stream::Span span;
        Stream::Span popped;

        EXPECT_TRUE(s.push(testData[0], defaultBlockSize));
        ASSERT_EQ(s.getPeekedCount(), 0);
        ASSERT_EQ(s.getUsedBlockCount(), 1);
        EXPECT_TRUE(s.peek(span));
        ASSERT_EQ(memcmp(span.data, testData[0], defaultBlockSize), 0);
        EXPECT_EQ(s.getUsedBlockCount(), 1);
        EXPECT_FALSE(s.pop(popped));
        EXPECT_EQ(s.getUsedBlockCount(), 1);

        s.unpeek();
        EXPECT_TRUE(s.peek(span));
        ASSERT_EQ(memcmp(span.data, testData[0], defaultBlockSize), 0);
        s.consume();
        EXPECT_EQ(s.getUsedBlockCount(), 0);
    }
}

TEST(Stream, GreedyPeek)
{
    StandardStreamAllocator a;
    Stream s(format, a, defaultBlockSize);
    Stream::Span span;

    initTestData();

    for (unsigned int i = 0; i < s.getBlockCount(); ++i) {
        ASSERT_TRUE(s.push(testData[i], defaultBlockSize));
    }

    ASSERT_EQ(s.getUsedBlockCount(), defaultBuffering);

    for (unsigned int i = 0; i < s.getBlockCount(); ++i) {
        EXPECT_EQ(s.getPeekedCount(), i);
        EXPECT_TRUE(s.peek(span));
        EXPECT_EQ(s.getPeekedCount(), i + 1);
        ASSERT_EQ(span.dataSize, defaultBlockSize);
        ASSERT_EQ(memcmp(span.data, testData[i], defaultBlockSize), 0);
    }

    ASSERT_FALSE(s.peek(span));
}

TEST(Stream, Reserve)
{
    StandardStreamAllocator a;
    Stream s(format, a, defaultBlockSize);
    Stream::Span span;

    EXPECT_EQ(s.getReservedCount(), 0);
    EXPECT_TRUE(s.reserve(span));
    EXPECT_EQ(s.getReservedCount(), 1);
    EXPECT_EQ(s.getUsedBlockCount(), 0);

    s.release();
    EXPECT_EQ(s.getReservedCount(), 0);
    EXPECT_EQ(s.getUsedBlockCount(), 0);

    for (unsigned int i = 0; i < s.getBlockCount(); ++i) {
        EXPECT_EQ(s.getReservedCount(), i);
        EXPECT_TRUE(s.reserve(span));
        EXPECT_EQ(s.getReservedCount(), i + 1);
    }

    EXPECT_FALSE(s.reserve(span));

    s.commit();
    EXPECT_EQ(s.getUsedBlockCount(), 1);
}

TEST(Stream, Iterator)
{
    std::uint8_t buf[defaultBlockSize * 2];

    Stream::RawBlockIterator it(buf, sizeof(buf), buf, 64);

    it++;
    auto data = *it;
    EXPECT_EQ(data.data, &buf[defaultBlockSize]);
    EXPECT_EQ(data.dataSize, defaultBlockSize);

    it++;
    data = *it;
    EXPECT_EQ(data.data, buf);
    EXPECT_EQ(data.dataSize, defaultBlockSize);

    it--;
    data = *it;
    EXPECT_EQ(data.data, &buf[defaultBlockSize]);
    EXPECT_EQ(data.dataSize, defaultBlockSize);

    it--;
    data = *it;
    EXPECT_EQ(data.data, buf);
    EXPECT_EQ(data.dataSize, defaultBlockSize);
}

TEST(Stream, Reset)
{
    StandardStreamAllocator a;
    Stream s(format, a, defaultBlockSize);

    s.push();
    ASSERT_FALSE(s.isEmpty());

    s.reset();
    EXPECT_TRUE(s.isEmpty());
}

TEST(Stream, spanEquality)
{
    std::uint8_t buf[defaultBlockSize];
    auto span  = Stream::Span{.data = buf, .dataSize = defaultBlockSize};
    auto span2 = span;
    EXPECT_TRUE(span == span2);
    EXPECT_FALSE(span != span2);

    span2.data = &buf[1];
    EXPECT_FALSE(span == span2);
    EXPECT_TRUE(span != span2);

    span2          = span;
    span2.dataSize = defaultBlockSize - 1;
    EXPECT_FALSE(span == span2);
    EXPECT_TRUE(span != span2);
}

TEST(Proxy, Write)
{
    auto mock  = std::make_shared<MockStream>();
    auto proxy = ::audio::StreamProxy(std::static_pointer_cast<::audio::AbstractStream>(mock));
    ::audio::AbstractStream::Span span{.data = reinterpret_cast<uint8_t *>(0xdeadbeef), .dataSize = 0xc00f33b4d};

    EXPECT_CALL(*mock, push(span)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(*mock, push(nullptr, 128)).WillOnce(Return(false));
    EXPECT_CALL(*mock, push()).WillOnce(Return(true));

    EXPECT_TRUE(proxy.push(span));
    EXPECT_FALSE(proxy.push(nullptr, 128));
    EXPECT_TRUE(proxy.push());

    EXPECT_CALL(*mock, reserve).Times(1);
    EXPECT_CALL(*mock, commit).Times(1);
    EXPECT_CALL(*mock, release).Times(1);

    proxy.reserve(span);
    proxy.commit();
    proxy.release();
}

TEST(Proxy, Read)
{
    auto mock  = std::make_shared<MockStream>();
    auto proxy = ::audio::StreamProxy(std::static_pointer_cast<::audio::AbstractStream>(mock));
    ::audio::AbstractStream::Span span{.data = reinterpret_cast<uint8_t *>(0xdeadbeef), .dataSize = 0xc00f33b4d};

    EXPECT_CALL(*mock, pop(span)).Times(1).WillOnce(Return(true));
    EXPECT_TRUE(proxy.pop(span));

    EXPECT_CALL(*mock, peek(span)).Times(2).WillOnce(Return(true)).WillOnce(Return(false));
    EXPECT_CALL(*mock, consume());
    EXPECT_CALL(*mock, unpeek());

    EXPECT_TRUE(proxy.peek(span));
    EXPECT_FALSE(proxy.peek(span));

    proxy.consume();
    proxy.unpeek();
}

TEST(Proxy, Misc)
{
    auto mock  = std::make_shared<MockStream>();
    auto proxy = ::audio::StreamProxy(std::static_pointer_cast<::audio::AbstractStream>(mock));

    EXPECT_CALL(*mock, reset).Times(1);
    EXPECT_CALL(*mock, isEmpty).Times(1).WillOnce(Return(true));
    EXPECT_CALL(*mock, isFull).Times(1).WillOnce(Return(false));

    proxy.reset();
    EXPECT_TRUE(proxy.isEmpty());
    EXPECT_FALSE(proxy.isFull());
}

TEST(Factory, BlockSizeConstraints)
{
    testing::audio::MockSink mockSink;
    testing::audio::MockSource mockSource;
    ::audio::StreamFactory factory(2ms);

    EXPECT_CALL(mockSink, getTraits).WillRepeatedly(Return(::audio::Endpoint::Traits{.blockSizeConstraint = 512U}));
    EXPECT_CALL(mockSource, getTraits).WillRepeatedly(Return(::audio::Endpoint::Traits{}));

    auto stream = factory.makeStream(mockSource, mockSink, format);

    EXPECT_EQ(stream->getOutputTraits().blockSize, 512U);
    EXPECT_EQ(stream->getInputTraits().blockSize, 512U);

    EXPECT_CALL(mockSink, getTraits).WillRepeatedly(Return(::audio::Endpoint::Traits{.blockSizeConstraint = 512U}));
    EXPECT_CALL(mockSource, getTraits).WillRepeatedly(Return(::audio::Endpoint::Traits{.blockSizeConstraint = 512U}));

    stream = factory.makeStream(mockSource, mockSink, format);

    EXPECT_EQ(stream->getOutputTraits().blockSize, 512U);
    EXPECT_EQ(stream->getInputTraits().blockSize, 512U);
}

TEST(Factory, BlockSizeErrors)
{
    testing::audio::MockSink mockSink;
    testing::audio::MockSource mockSource;
    ::audio::StreamFactory factory(2ms);

    EXPECT_CALL(mockSource, getTraits).WillRepeatedly(Return(::audio::Endpoint::Traits{.blockSizeConstraint = 128U}));
    EXPECT_CALL(mockSink, getTraits).WillRepeatedly(Return(::audio::Endpoint::Traits{.blockSizeConstraint = 300U}));
    EXPECT_THROW(factory.makeStream(mockSource, mockSink, format), std::invalid_argument);

    EXPECT_CALL(mockSource, getTraits).WillRepeatedly(Return(::audio::Endpoint::Traits{.blockSizeConstraint = 128U}));
    EXPECT_CALL(mockSink, getTraits).WillRepeatedly(Return(::audio::Endpoint::Traits{.blockSizeConstraint = 0}));
    EXPECT_THROW(factory.makeStream(mockSource, mockSink, format), std::invalid_argument);
}

TEST(Factory, TimeContraintsError)
{
    testing::audio::MockSink mockSink;
    testing::audio::MockSource mockSource;
    ::audio::StreamFactory factory;

    EXPECT_CALL(mockSource, getTraits).WillRepeatedly(Return(::audio::Endpoint::Traits{}));
    EXPECT_CALL(mockSink, getTraits).WillRepeatedly(Return(::audio::Endpoint::Traits{}));

    EXPECT_THROW(factory.makeStream(mockSource, mockSink, format), std::invalid_argument);
}

TEST(Factory, NoSourceFormat)
{
    testing::audio::MockSink mockSink;
    testing::audio::MockSource mockSource;
    ::audio::StreamFactory factory;

    EXPECT_CALL(mockSource, getTraits).WillRepeatedly(Return(::audio::Endpoint::Traits{}));
    EXPECT_CALL(mockSink, getTraits).WillRepeatedly(Return(::audio::Endpoint::Traits{}));
    EXPECT_THROW(factory.makeStream(mockSource, mockSink, ::audio::nullFormat), std::invalid_argument);
}

TEST(Factory, TimeConstraints)
{
    testing::audio::MockSink mockSink;
    testing::audio::MockSource mockSource;
    ::audio::StreamFactory factory(10ms);

    EXPECT_CALL(mockSource, getTraits).WillRepeatedly(Return(::audio::Endpoint::Traits{}));
    EXPECT_CALL(mockSink, getTraits)
        .WillRepeatedly(Return(::audio::Endpoint::Traits{.blockSizeConstraint = 512U, .timeConstraint = 10ms}));

    auto stream = factory.makeStream(mockSource, mockSink, ::audio::AudioFormat(44100, 16, 2));

    EXPECT_EQ(stream->getBlockCount(), defaultBuffering * 4);
    EXPECT_EQ(stream->getOutputTraits().blockSize, 512);
}

TEST(Factory, TimeBlockConstraints)
{
    testing::audio::MockSink mockSink;
    testing::audio::MockSource mockSource;
    ::audio::StreamFactory factory(2ms);

    EXPECT_CALL(mockSource, getTraits).WillRepeatedly(Return(::audio::Endpoint::Traits{}));
    EXPECT_CALL(mockSink, getTraits).WillRepeatedly(Return(::audio::Endpoint::Traits{.blockSizeConstraint = 60U}));

    auto stream = factory.makeStream(mockSource, mockSink, ::audio::AudioFormat(8000, 16, 1));

    EXPECT_EQ(stream->getBlockCount(), defaultBuffering);
    EXPECT_EQ(stream->getOutputTraits().blockSize, 60);
}

TEST(Factory, NoConstraints)
{
    testing::audio::MockSink mockSink;
    testing::audio::MockSource mockSource;
    ::audio::StreamFactory factory(2ms);

    EXPECT_CALL(mockSource, getTraits).WillRepeatedly(Return(::audio::Endpoint::Traits{}));
    EXPECT_CALL(mockSink, getTraits).WillRepeatedly(Return(::audio::Endpoint::Traits{}));

    auto stream = factory.makeStream(mockSource, mockSink, ::audio::AudioFormat(16000, 16, 1));

    EXPECT_EQ(stream->getBlockCount(), defaultBuffering);
    EXPECT_EQ(stream->getOutputTraits().blockSize, 64);
}

TEST(Factory, TranscodingStreamSinkTraits)
{
    testing::audio::MockSink mockSink;
    testing::audio::MockSource mockSource;
    ::audio::StreamFactory factory(2ms);
    auto format = ::audio::AudioFormat(8000, 16, 1);

    EXPECT_CALL(mockSource, getTraits).WillRepeatedly(Return(::audio::Endpoint::Traits{}));
    EXPECT_CALL(mockSink, getTraits).WillRepeatedly(Return(::audio::Endpoint::Traits{.blockSizeConstraint = 60U}));

    auto stream = factory.makeStream(mockSource, mockSink, format);

    EXPECT_EQ(stream->getBlockCount(), defaultBuffering);
    EXPECT_EQ(stream->getOutputTraits().blockSize, 60);

    auto decimatorTransform = std::make_shared<::audio::transcode::BasicDecimator<std::uint16_t, 1, 2>>();
    auto transcodingStream  = factory.makeInputTranscodingStream(
        mockSource, mockSink, format, std::static_pointer_cast<::audio::transcode::Transform>(decimatorTransform));

    EXPECT_EQ(transcodingStream->getInputTraits().blockSize, 120);
    EXPECT_EQ(transcodingStream->getOutputTraits().blockSize, 60);
}

TEST(Factory, TranscodingStreamSourceTraits)
{
    testing::audio::MockSink mockSink;
    testing::audio::MockSource mockSource;
    ::audio::StreamFactory factory(2ms);
    auto format = ::audio::AudioFormat(8000, 16, 1);

    EXPECT_CALL(mockSource, getTraits).WillRepeatedly(Return(::audio::Endpoint::Traits{.blockSizeConstraint = 60U}));
    EXPECT_CALL(mockSink, getTraits).WillRepeatedly(Return(::audio::Endpoint::Traits{}));

    auto stream = factory.makeStream(mockSource, mockSink, format);

    EXPECT_EQ(stream->getBlockCount(), defaultBuffering);
    EXPECT_EQ(stream->getOutputTraits().blockSize, 60);

    auto decimatorTransform = std::make_shared<::audio::transcode::BasicDecimator<std::uint16_t, 1, 2>>();
    auto transcodingStream  = factory.makeInputTranscodingStream(
        mockSource, mockSink, format, std::static_pointer_cast<::audio::transcode::Transform>(decimatorTransform));

    EXPECT_EQ(transcodingStream->getInputTraits().blockSize, 60);
    EXPECT_EQ(transcodingStream->getOutputTraits().blockSize, 30);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
