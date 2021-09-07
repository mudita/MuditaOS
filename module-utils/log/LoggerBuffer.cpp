// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LoggerBuffer.hpp"

std::pair<bool, std::string> LoggerBuffer::get()
{
    auto [result, logMsg] = StringCircularBuffer::get();
    if (!result) {
        return {result, logMsg};
    }
    if (numOfLostBytes > 0) {
        logMsg         = std::to_string(numOfLostBytes) + " " + lostBytesMessage + "\n" + logMsg;
        numOfLostBytes = 0;
    }
    return {true, logMsg};
}

void LoggerBuffer::put(const std::string &logMsg)
{
    updateNumOfLostBytes();
    StringCircularBuffer::put(logMsg);
}

void LoggerBuffer::put(std::string &&logMsg)
{
    updateNumOfLostBytes();
    StringCircularBuffer::put(std::move(logMsg));
}

void LoggerBuffer::updateNumOfLostBytes()
{
    if (StringCircularBuffer::isFull()) {
        auto [_, lostMsg] = StringCircularBuffer::get();
        numOfLostBytes += lostMsg.length();
    }
}
