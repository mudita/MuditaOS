
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

class Field {
public:

    Field() : mValue(nullptr) {}

    Field(const char *value) : mValue(value) {}
    //Field(Field const&);

    // Field& operator=(Field const&);



    ~Field() {}

    const char *GetCString() const { return mValue.c_str(); }

    std::string GetString() const {
        return mValue;
    }

    float GetFloat() const {
        return !mValue.empty() ? static_cast<float>(std::stof(mValue)) : 0.0f;
    }

    bool GetBool() const {
        return !mValue.empty() ? std::stoi(mValue) > 0 : false;
    }

    double GetDouble() const {
        return !mValue.empty() ? static_cast<double>(std::stod(mValue)) : 0.0f;
    }

    int8_t GetInt8() const {
        return !mValue.empty() ? static_cast<int8_t>(atol(mValue.c_str())) : int8_t(0);
    }

    int32_t GetInt32() const {
        return !mValue.empty() ? static_cast<int32_t>(atol(mValue.c_str())) : int32_t(0);
    }

    uint8_t GetUInt8() const {
        return !mValue.empty() ? static_cast<uint8_t>(atol(mValue.c_str())) : uint8_t(0);
    }

    uint16_t GetUInt16() const {
        return !mValue.empty() ? static_cast<uint16_t>(atol(mValue.c_str())) : uint16_t(0);
    }

    int16_t GetInt16() const {
        return !mValue.empty() ? static_cast<int16_t>(atol(mValue.c_str())) : int16_t(0);
    }

    uint32_t GetUInt32() const {
        return !mValue.empty() ? std::stoul(mValue) : uint32_t(0);
    }

    uint64_t GetUInt64() const {
        return !mValue.empty() ? std::stoull(mValue) : uint64_t(0);
    }

    uint64_t GetInt64() const {
        return !mValue.empty() ? std::stoll(mValue) : int64_t(0);
    }


    void SetValue(const char *value) { mValue = value; }

private:
    std::string mValue;
};


#endif //PUREPHONE_FIELD_HPP
