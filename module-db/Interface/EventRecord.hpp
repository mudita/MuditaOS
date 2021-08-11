// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Record.hpp"

#include <module-db/Common/Common.hpp>
#include <Tables/Record.hpp>
#include <Tables/EventsTable.hpp>

#include <time/dateCommon.hpp>

#include <utf8/UTF8.hpp>

#include <stdint.h>
#include <string>

struct EventInfo
{
    UTF8 name;
    TimePoint startDate{TIME_POINT_INVALID};
    TimePoint endDate{TIME_POINT_INVALID};
    uint32_t duration{0};
    bool isAllDay{false};

    EventInfo() = default;
    EventInfo(UTF8 name, TimePoint startDate, TimePoint endDate, uint32_t duration, bool isAllDay)
        : name{name}, startDate{startDate}, endDate{endDate}, duration{duration}, isAllDay{isAllDay} {};
};

struct SingleEventRecord;

struct EventRecord : public Record, public EventInfo
{
    std::string rrule{""};

    EventRecord() = default;
    EventRecord(uint32_t id,
                UTF8 name,
                TimePoint startDate,
                TimePoint endDate,
                uint32_t duration,
                bool isAllDay,
                std::string rrule);
    EventRecord(EventRecord *er);

    std::vector<SingleEventRecord> generateSingleEvents(TimePoint from, TimePoint to, uint32_t count);
    SingleEventRecord getNextSingleEvent(TimePoint from);
};

struct SingleEventRecord : public Record, public EventInfo
{
    std::shared_ptr<EventRecord> parent;

    SingleEventRecord() = default;
    SingleEventRecord(std::shared_ptr<EventRecord> parent, TimePoint startDate, TimePoint endDate)
        : EventInfo{parent->name, startDate, endDate, parent->duration, parent->isAllDay}, parent{parent} {};
};
