
/*
 * @file Field.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 24.05.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

#ifndef PUREPHONE_FIELD_HPP
#define PUREPHONE_FIELD_HPP

#include <stdint.h>
#include <string>

class Field
{
  public:
    Field() : mValue(nullptr)
    {
    }

    Field(const char *value)
    {
        if (value)
        {
            mValue = value;
        }
    }
    // Field(Field const&);

    // Field& operator=(Field const&);

    ~Field()
    {
    }

    const char *GetCString() const;
    std::string GetString() const { return mValue; }
    float GetFloat() const;
    bool GetBool() const;
    double GetDouble() const;
    int8_t GetInt8() const;
    int32_t GetInt32() const;
    uint8_t GetUInt8() const;
    uint16_t GetUInt16() const;
    int16_t GetInt16() const;
    uint32_t GetUInt32() const;
    uint64_t GetUInt64() const;
    int64_t GetInt64() const;
    void SetValue(const char *value);

  private:
    std::string mValue;
};

#endif // PUREPHONE_FIELD_HPP
