// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <gtest/gtest.h>

#include <Audio/Stream.hpp>

#include <cstdint>
#include <cstring>

constexpr std::size_t defaultBlockSize = 64U;
constexpr std::size_t defaultBuffering = 4U;

using namespace audio;

static std::uint8_t testData[defaultBuffering][defaultBlockSize];
static std::uint8_t emptyBlock[defaultBlockSize];

#include <iostream>

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
    Stream s(a, defaultBlockSize, bufferingSize);

    EXPECT_EQ(s.getBlockCount(), bufferingSize);
    EXPECT_EQ(s.getBlockSize(), defaultBlockSize);
    EXPECT_EQ(s.getUsedBlockCount(), 0);
}

TEST(Stream, Push)
{
    StandardStreamAllocator a;
    Stream s(a, defaultBlockSize);
    auto block = testData[0];

    EXPECT_TRUE(s.push(block, defaultBlockSize));
    EXPECT_TRUE(s.push(block, defaultBlockSize));
    EXPECT_TRUE(s.push(block, defaultBlockSize));
    EXPECT_TRUE(s.push(block, defaultBlockSize));
    EXPECT_EQ(s.getUsedBlockCount(), 4);
    EXPECT_FALSE(s.push(block, defaultBlockSize));
}

TEST(Stream, PushPop)
{
    StandardStreamAllocator a;
    Stream s(a, defaultBlockSize);

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
    Stream s(a, defaultBlockSize);

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
    Stream s(a, defaultBlockSize);
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
    Stream s(a, defaultBlockSize);
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

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
