// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "LoggerBuffer.hpp"

class LoggerBufferContainer
{
  public:
    LoggerBufferContainer()
        : currentIndex{0}, buffer{LoggerBuffer(circularBufferSize), LoggerBuffer(circularBufferSize)},
          currentBuffer{buffer}, flushBuffer{buffer}
    {}

    LoggerBuffer *getFlushBuffer()
    {
        return flushBuffer;
    }

    LoggerBuffer *getCurrentBuffer()
    {
        return currentBuffer;
    }

    constexpr size_t getCircularBufferSize()
    {
        return circularBufferSize;
    }

    void nextBuffer()
    {
        ++currentIndex;
        flushBuffer = currentBuffer;
        currentIndex %= numberOfBuffers;
        currentBuffer = &buffer[currentIndex];
    }

    size_t getCurrentIndex()
    {
        return currentIndex;
    }

  private:
    static constexpr size_t circularBufferSize = 100;
    static constexpr size_t numberOfBuffers    = 2;

    size_t currentIndex;
    LoggerBuffer buffer[numberOfBuffers];
    LoggerBuffer *currentBuffer;
    LoggerBuffer *flushBuffer;
};
