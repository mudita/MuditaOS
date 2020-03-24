#pragma once

#include <cstdint>

constexpr uint32_t DB_ID_NONE = 0;

struct Record
{
    uint32_t ID = DB_ID_NONE;
};
