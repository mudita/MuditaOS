// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

#include "RandomStringGenerator.hpp"
#include "LoggerBuffer.hpp"
#include <string>
#include <vector>

using namespace std;

void TestBuffer(const LoggerBuffer &buffer, size_t capacity, size_t numOfMsgs)
{
    const bool isEmpty = buffer.isEmpty();
    REQUIRE((numOfMsgs == 0 ? isEmpty : !isEmpty));
    const bool isFull = buffer.isFull();
    REQUIRE((capacity > 0 && capacity == numOfMsgs ? isFull : !isFull));
    REQUIRE(buffer.getCapacity() == capacity);
    REQUIRE(buffer.getSize() == numOfMsgs);
}

size_t GetNumOfBytes(vector<string>::const_iterator startIt, vector<string>::const_iterator endIt)
{
    size_t numOfBytes = 0;
    for (; startIt != endIt; ++startIt) {
        numOfBytes += startIt->length();
    }
    return numOfBytes;
}

TEST_CASE("LoggerBuffer tests")
{
    const size_t capacity = 100;
    LoggerBuffer buffer(capacity);

    RandomStringGenerator randomStringGenerator;

    auto putMsgFunc = [&](const auto &msg) { buffer.put(msg); };
    auto getMsgFunc = [&](const auto &originalMsg) {
        const auto [result, msg] = buffer.get();
        REQUIRE(result);
        REQUIRE(msg == originalMsg);
    };
    auto putAllMsgsFunc = [&](const vector<string> &msgs) { for_each(msgs.begin(), msgs.end(), putMsgFunc); };
    auto getAllMsgsFunc = [&](const vector<string> &msgs) { for_each(msgs.begin(), msgs.end(), getMsgFunc); };
    auto checkLostBytes = [&](size_t numOfBytes, const string &originalMsg) {
        const auto [result, msg] = buffer.get();
        REQUIRE(result);
        REQUIRE(msg.find(originalMsg) != string::npos);
        REQUIRE(msg.find(to_string(numOfBytes)) != string::npos);
        REQUIRE(msg.find(LoggerBuffer::lostBytesMessage) != string::npos);
    };

    SECTION("calling get on empty buffer should return false")
    {
        const auto [result, _] = buffer.get();
        REQUIRE(!result);
        TestBuffer(buffer, capacity, 0);
    }

    SECTION("after putting one msg in buffer, get should return this msg")
    {
        const string originalMsg = randomStringGenerator.getRandomString();
        buffer.put(originalMsg);
        TestBuffer(buffer, capacity, 1);
        const auto [result, msg] = buffer.get();
        REQUIRE(result);
        REQUIRE(msg == originalMsg);
        TestBuffer(buffer, capacity, 0);
    }

    SECTION("after filling whole buffer with msgs, caliling get repeatedly should return all these msgs back")
    {
        const auto msgs = randomStringGenerator.createRandomStringVector(capacity);
        putAllMsgsFunc(msgs);
        TestBuffer(buffer, capacity, msgs.size());
        getAllMsgsFunc(msgs);
        TestBuffer(buffer, capacity, 0);
    }

    SECTION("buffer should be empty after resetting it")
    {
        const auto msgs = randomStringGenerator.createRandomStringVector(capacity);
        putAllMsgsFunc(msgs);
        TestBuffer(buffer, capacity, msgs.size());
        buffer.reset();
        TestBuffer(buffer, capacity, 0);
    }

    SECTION("when more msgs are put into buffer than its capacity, only last msgs should be returned with get and "
            "first msg should contain lost bytes message")
    {
        const size_t numOfMsgsAboveCapacity = 13;
        const auto msgs = randomStringGenerator.createRandomStringVector(capacity + numOfMsgsAboveCapacity);
        putAllMsgsFunc(msgs);
        auto firstLostMsgIt       = msgs.begin();
        auto firstMsgInBufferIt   = firstLostMsgIt + numOfMsgsAboveCapacity;
        const auto numOfLostBytes = GetNumOfBytes(firstLostMsgIt, firstMsgInBufferIt);
        checkLostBytes(numOfLostBytes, *firstMsgInBufferIt);
        for_each(firstMsgInBufferIt + 1, msgs.end(), getMsgFunc);
        TestBuffer(buffer, capacity, 0);
    }

    SECTION("each get should reduce number of msgs in buffer")
    {
        size_t numOfMsgsInBuffer = capacity;
        const auto msgs          = randomStringGenerator.createRandomStringVector(capacity);

        auto getStartIt = msgs.begin();

        putAllMsgsFunc(msgs);
        TestBuffer(buffer, capacity, capacity);

        size_t numOfMsgsToGet = 15;
        for_each(getStartIt, getStartIt + numOfMsgsToGet, getMsgFunc);
        numOfMsgsInBuffer -= numOfMsgsToGet;
        TestBuffer(buffer, capacity, numOfMsgsInBuffer);
        getStartIt += numOfMsgsToGet;

        numOfMsgsToGet = 34;
        for_each(getStartIt, getStartIt + numOfMsgsToGet, getMsgFunc);
        numOfMsgsInBuffer -= numOfMsgsToGet;
        TestBuffer(buffer, capacity, numOfMsgsInBuffer);
        getStartIt += numOfMsgsToGet;

        for_each(getStartIt, msgs.end(), getMsgFunc);
        TestBuffer(buffer, capacity, 0);
    }

    SECTION("put get put get")
    {
        const auto msgs = randomStringGenerator.createRandomStringVector(capacity);

        auto putStartIt = msgs.begin();
        auto getStartIt = msgs.begin();

        size_t numOfMsgsToPut = 37;
        for_each(putStartIt, putStartIt + numOfMsgsToPut, putMsgFunc);
        putStartIt += numOfMsgsToPut;
        size_t numOfMsgsInBuffer = numOfMsgsToPut;
        TestBuffer(buffer, capacity, numOfMsgsInBuffer);

        size_t numOfMsgsToGet = 25;
        for_each(getStartIt, getStartIt + numOfMsgsToGet, getMsgFunc);
        getStartIt += numOfMsgsToGet;
        numOfMsgsInBuffer -= numOfMsgsToGet;
        TestBuffer(buffer, capacity, numOfMsgsInBuffer);

        numOfMsgsToPut = msgs.end() - putStartIt;
        for_each(putStartIt, msgs.end(), putMsgFunc);
        numOfMsgsInBuffer += numOfMsgsToPut;
        TestBuffer(buffer, capacity, numOfMsgsInBuffer);

        for_each(getStartIt, msgs.end(), getMsgFunc);
        TestBuffer(buffer, capacity, 0);
    }

    SECTION("put get put get - with buffer overflow")
    {
        const size_t numOfMsgsAboveCapacity = 43;
        const auto msgs = randomStringGenerator.createRandomStringVector(capacity + numOfMsgsAboveCapacity);

        auto putStartIt = msgs.begin();
        auto getStartIt = msgs.begin();

        size_t numOfMsgsToPut = 77;
        for_each(putStartIt, putStartIt + numOfMsgsToPut, putMsgFunc);
        putStartIt += numOfMsgsToPut;
        size_t numOfMsgsInBuffer = numOfMsgsToPut;
        TestBuffer(buffer, capacity, numOfMsgsInBuffer);

        size_t numOfMsgsToGet = 15;
        for_each(getStartIt, getStartIt + numOfMsgsToGet, getMsgFunc);
        getStartIt += numOfMsgsToGet;
        numOfMsgsInBuffer -= numOfMsgsToGet;
        TestBuffer(buffer, capacity, numOfMsgsInBuffer);

        numOfMsgsToPut = msgs.end() - putStartIt; // put rest of msgs - more than buffer can hold
        for_each(putStartIt, msgs.end(), putMsgFunc);
        numOfMsgsInBuffer = capacity;
        TestBuffer(buffer, capacity, numOfMsgsInBuffer);

        auto firstLostMsgIt       = getStartIt;
        auto firstMsgInBufferIt   = firstLostMsgIt + numOfMsgsAboveCapacity - numOfMsgsToGet;
        const auto numOfLostBytes = GetNumOfBytes(firstLostMsgIt, firstMsgInBufferIt);
        checkLostBytes(numOfLostBytes, *firstMsgInBufferIt);
        for_each(firstMsgInBufferIt + 1, msgs.end(), getMsgFunc);
        TestBuffer(buffer, capacity, 0);
    }
}
