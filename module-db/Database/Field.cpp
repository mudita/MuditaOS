// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Field.hpp"
#include <log/log.hpp>
#include <functional>

const char *Field::getCString() const
{
    return mValue.c_str();
}

const std::string &Field::getString() const
{
    return mValue;
}

template <class T>
static T lget(T &val, const std::string &mValue, std::function<T()> foo)
{
    val = 0;
    if (mValue.empty()) {
        return val;
    }
    try {
        return foo();
    }
    catch (...) {
        LOG_FATAL("Can't convert value to: %s", typeid(T).name());
    }
    return val;
}

float Field::getFloat() const
{
    float ret;
    return lget<float>(ret, mValue, [=]() -> float { return std::stof(mValue); });
}

bool Field::getBool() const
{
    bool ret;
    return lget<bool>(ret, mValue, [=]() -> bool { return std::stoi(mValue) > 0; });
}

double Field::getDouble() const
{
    double ret;
    return lget<double>(ret, mValue, [=]() -> double { return std::stod(mValue); });
}

std::int8_t Field::getInt8() const
{
    return !mValue.empty() ? static_cast<std::int8_t>(atol(mValue.c_str())) : static_cast<std::int8_t>(0);
}

std::int32_t Field::getInt32() const
{
    return !mValue.empty() ? static_cast<std::int32_t>(atol(mValue.c_str())) : static_cast<std::int32_t>(0);
}

std::uint8_t Field::getUInt8() const
{
    return !mValue.empty() ? static_cast<std::uint8_t>(atol(mValue.c_str())) : static_cast<std::uint8_t>(0);
}

std::uint16_t Field::getUInt16() const
{
    return !mValue.empty() ? static_cast<std::uint16_t>(atol(mValue.c_str())) : static_cast<std::uint16_t>(0);
}

std::int16_t Field::getInt16() const
{
    return !mValue.empty() ? static_cast<std::int16_t>(atol(mValue.c_str())) : static_cast<std::int16_t>(0);
}

std::uint32_t Field::getUInt32() const
{
    std::uint32_t ret;
    return lget<std::uint32_t>(ret, mValue, [=]() -> unsigned long { return std::stoul(mValue); });
}

std::uint64_t Field::getUInt64() const
{
    std::uint64_t ret;
    return lget<std::uint64_t>(ret, mValue, [=]() -> unsigned long long { return std::stoull(mValue); });
}

std::int64_t Field::getInt64() const
{
    std::int64_t ret;
    return lget<std::int64_t>(ret, mValue, [=]() -> long long { return std::stoll(mValue); });
}

void Field::setValue(const char *value)
{
    mValue = value;
}
