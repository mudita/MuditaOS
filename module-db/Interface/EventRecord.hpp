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
        : name{name}, startDate{startDate}, endDate{endDate}, duration{duration}, isAllDay{isAllDay} {};

    auto isValid() const -> bool;
};

struct SingleEventRecord;

struct EventRecord : public Record, public EventInfo
{
    std::string rruleText{""};

    EventRecord() = default;
    EventRecord(uint32_t id,
                const UTF8 &name,
                TimePoint startDate,
                uint32_t duration,
                bool isAllDay,
                const std::string &rruleText);

    virtual ~EventRecord(){};
    explicit EventRecord(EventRecord *record);

    auto generateSingleEvents(TimePoint from, TimePoint to, uint32_t count) -> std::vector<SingleEventRecord>;
    auto getNextSingleEvent(TimePoint from) -> SingleEventRecord;
    auto isValid() const -> bool;

    virtual std::shared_ptr<EventRecord> getCopy();
};

struct SingleEventRecord : public Record, public EventInfo
{
    std::shared_ptr<EventRecord> parent;

    virtual ~SingleEventRecord() = default;

    SingleEventRecord() = default;
    SingleEventRecord(std::shared_ptr<EventRecord> parent, TimePoint startDate, TimePoint endDate)
        : EventInfo{parent->name, startDate, endDate, parent->duration, parent->isAllDay}, parent{parent} {};
};
