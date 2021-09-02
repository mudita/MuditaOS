// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Record.hpp"
#include "Table.hpp"

#include <Common/Common.hpp>
#include <Database/Database.hpp>

#include <time/dateCommon.hpp>

#include <utf8/UTF8.hpp>

struct EventRecord;

struct EventsTableRow : public Record
{
    UTF8 name{""};
    TimePoint startDate{TIME_POINT_INVALID};
    TimePoint endDate{TIME_POINT_INVALID};
    uint32_t duration{0};
    bool isAllDay{false};
    std::string rruleText{""};

    EventsTableRow() = default;
    EventsTableRow(uint32_t id,
                   UTF8 name,
                   TimePoint startDate,
                   TimePoint endDate,
                   uint32_t duration,
                   bool isAllDay,
                   std::string rruleText)
        : Record{id}, name{name}, startDate{startDate}, endDate{endDate}, duration{duration}, isAllDay{isAllDay},
          rruleText{rruleText} {};
};
