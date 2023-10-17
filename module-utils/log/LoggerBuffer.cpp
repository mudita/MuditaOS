// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LoggerBuffer.hpp"

std::optional<std::string> LoggerBuffer::get()
{
    auto logMsg = StringCircularBuffer::get();
    if (!logMsg.has_value()) {
        return std::nullopt;
    }
    if (numOfLostBytes > 0) {
        logMsg         = std::to_string(numOfLostBytes) + " " + lostBytesMessage + "\n" + logMsg.value();
        numOfLostBytes = 0;
    }
    return logMsg;
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
    if (!StringCircularBuffer::isFull()) {
        return;
    }

    const auto lostMsg = StringCircularBuffer::get();
    if (lostMsg.has_value()) {
        numOfLostBytes += lostMsg.value().length();
    }
}
