// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
        if (value) {
            mValue = value;
        }
    }
    // Field(Field const&);

    // Field& operator=(Field const&);

    ~Field()
    {}

    const char *getCString() const;
    std::string getString() const
    {
        return mValue;
    }
    float getFloat() const;
    bool getBool() const;
    double getDouble() const;
    int8_t getInt8() const;
    int32_t getInt32() const;
    uint8_t getUInt8() const;
    uint16_t getUInt16() const;
    int16_t getInt16() const;
    uint32_t getUInt32() const;
    uint64_t getUInt64() const;
    int64_t getInt64() const;
    void setValue(const char *value);

  private:
    std::string mValue;
};
