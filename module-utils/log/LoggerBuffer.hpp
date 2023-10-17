// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "StringCircularBuffer.hpp"

class LoggerBuffer : public StringCircularBuffer
{
  public:
    explicit LoggerBuffer(size_t size) : StringCircularBuffer(size)
    {}

    [[nodiscard]] std::optional<std::string> get();
    void put(const std::string &logMsg);
    void put(std::string &&logMsg);

    static constexpr auto lostBytesMessage = "bytes were lost.";

  private:
    void updateNumOfLostBytes();

    size_t numOfLostBytes{0};
};
