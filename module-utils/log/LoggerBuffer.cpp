// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LoggerBuffer.hpp"

namespace Log
{
    std::pair<bool, std::string> LoggerBuffer::get()
    {
        auto [result, logMsg] = StringCircularBuffer::get();
        if (!result) {
            return {result, logMsg};
        }
        if (numOfLostBytes > 0) {
            logMsg += "\r\n" + std::to_string(numOfLostBytes) + " " + lostBytesMessage;
            numOfLostBytes = 0;
        }
        numOfBytesInBuffer -= logMsg.length();
        return {true, logMsg};
    }

    void LoggerBuffer::put(const std::string &logMsg)
    {
        numOfBytesInBuffer += logMsg.length();
        updateNumOfLostBytes();
        StringCircularBuffer::put(logMsg);
    }

    void LoggerBuffer::put(std::string &&logMsg)
    {
        numOfBytesInBuffer += logMsg.length();
        updateNumOfLostBytes();
        StringCircularBuffer::put(std::move(logMsg));
    }

    void LoggerBuffer::updateNumOfLostBytes()
    {
        if (StringCircularBuffer::isFull()) {
            auto [_, lostMsg] = StringCircularBuffer::get();
            numOfLostBytes += lostMsg.length();
            numOfBytesInBuffer -= numOfLostBytes;
        }
    }
} // namespace Log
