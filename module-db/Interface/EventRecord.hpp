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
    EventInfo(const UTF8 &name, TimePoint startDate, TimePoint endDate, uint32_t duration, bool isAllDay)
        : name{name}, startDate{TimePointFloorMinutes(startDate)}, endDate{TimePointFloorMinutes(endDate)},
          duration{duration}, isAllDay{isAllDay} {};
    EventInfo(TimePoint startDate, TimePoint endDate)
        : startDate{TimePointFloorMinutes(startDate)}, endDate{TimePointFloorMinutes(endDate)} {};

    auto isValid() const -> bool;
};

struct SingleEventRecord;

struct EventRecord : public Record
{
    EventRecord() = default;
    explicit EventRecord(uint32_t id);

    virtual ~EventRecord(){};
    explicit EventRecord(EventRecord *record);

    auto isValid() const -> bool;

    virtual std::shared_ptr<EventRecord> getCopy();
};

struct SingleEventRecord : public Record, public EventInfo
{
    std::shared_ptr<EventRecord> parent;

    virtual ~SingleEventRecord() = default;

    SingleEventRecord() = default;
    SingleEventRecord(std::shared_ptr<EventRecord> parent, TimePoint startDate, TimePoint endDate)
        : EventInfo{startDate, endDate}, parent{parent} {};

    auto isValid() const -> bool;
};
