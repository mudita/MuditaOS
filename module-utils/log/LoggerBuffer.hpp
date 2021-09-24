// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "StringCircularBuffer.hpp"

class LoggerBuffer : public StringCircularBuffer
{
  public:
    using StringCircularBuffer::StringCircularBuffer;

    [[nodiscard]] std::pair<bool, std::string> get() override;
    void put(const std::string &logMsg) override;
    void put(std::string &&logMsg) override;

    static constexpr auto lostBytesMessage = "bytes was lost.";

  private:
    void updateNumOfLostBytes();

    size_t numOfLostBytes{0};
};
