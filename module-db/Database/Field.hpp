// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <stdint.h>
#include <string>

class Field
{
  public:
    Field() : mValue(nullptr)
    {}

    Field(const char *value)
    {
        if (value != nullptr) {
            mValue = value;
        }
    }

    ~Field()
    {}

    const char *getCString() const;
    const std::string &getString() const;
    float getFloat() const;
    bool getBool() const;
    double getDouble() const;
    std::int8_t getInt8() const;
    std::int32_t getInt32() const;
    std::uint8_t getUInt8() const;
    std::uint16_t getUInt16() const;
    std::int16_t getInt16() const;
    std::uint32_t getUInt32() const;
    std::uint64_t getUInt64() const;
    std::int64_t getInt64() const;
    void setValue(const char *value);

  private:
    std::string mValue;
};
