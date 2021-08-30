// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <log.hpp>
#include <cstdint>

constexpr uint32_t DB_ID_NONE = 0;

struct Record
{
    uint32_t ID = DB_ID_NONE;

    Record() = default;
    Record(uint32_t ID) : ID(ID){};

    bool isValid() const
    {
        auto result = ID != DB_ID_NONE;
        if (!result) {
            LOG_DEBUG("Record validation failed - incorrect ID");
        }
        return result;
    }
};
