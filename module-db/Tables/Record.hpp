#pragma once

#include <log/log.hpp>
#include <cstdint>

constexpr uint32_t DB_ID_NONE = 0;

struct Record
{
    uint32_t ID = DB_ID_NONE;

    bool isValid() const
    {
        auto result = ID != DB_ID_NONE;
        if (!result) {
            LOG_DEBUG("Record validation failed - incorrect ID");
        }
        return result;
    }
};
