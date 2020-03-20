#include "Field.hpp"
#include <segger/log/log.hpp>
#include <functional>

const char *Field::GetCString() const
{
    return mValue.c_str();
}

template <class T> static T lget(T &val, const std::string &mValue, std::function<T()> foo)
{
    val = 0;
    if (mValue.empty()) {
        return val;
    }
    try {
        return foo();
    }
    catch (...) {
        LOG_FATAL("Can't convert: %s to: %s", mValue.c_str(), typeid(T).name());
    }
    return val;
}

float Field::GetFloat() const
{
    float ret;
    return lget<float>(ret, mValue, [=]() -> float { return std::stof(mValue); });
}

bool Field::GetBool() const
{
    bool ret;
    return lget<bool>(ret, mValue, [=]() -> bool { return std::stoi(mValue) > 0; });
}

double Field::GetDouble() const
{
    double ret;
    return lget<double>(ret, mValue, [=]() -> double { return std::stod(mValue); });
}

int8_t Field::GetInt8() const
{
    return !mValue.empty() ? static_cast<int8_t>(atol(mValue.c_str())) : int8_t(0);
}

int32_t Field::GetInt32() const
{
    return !mValue.empty() ? static_cast<int32_t>(atol(mValue.c_str())) : int32_t(0);
}

uint8_t Field::GetUInt8() const
{
    return !mValue.empty() ? static_cast<uint8_t>(atol(mValue.c_str())) : uint8_t(0);
}

uint16_t Field::GetUInt16() const
{
    return !mValue.empty() ? static_cast<uint16_t>(atol(mValue.c_str())) : uint16_t(0);
}

int16_t Field::GetInt16() const
{
    return !mValue.empty() ? static_cast<int16_t>(atol(mValue.c_str())) : int16_t(0);
}

uint32_t Field::GetUInt32() const
{
    uint32_t ret;
    return lget<uint32_t>(ret, mValue, [=]() -> unsigned long { return std::stoul(mValue); });
}

uint64_t Field::GetUInt64() const
{
    uint64_t ret;
    return lget<uint64_t>(ret, mValue, [=]() -> unsigned long long { return std::stoull(mValue); });
}

int64_t Field::GetInt64() const
{
    int64_t ret;
    return lget<int64_t>(ret, mValue, [=]() -> long long { return std::stoll(mValue); });
}

void Field::SetValue(const char *value)
{
    mValue = value;
}
