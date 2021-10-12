// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EventRecord.hpp"

#include <rrule/rrule.hpp>

bool EventInfo::isValid() const
{
    return !(startDate == TIME_POINT_INVALID || startDate == TIME_POINT_MAX || endDate == TIME_POINT_INVALID);
}

EventRecord::EventRecord(uint32_t id) : Record{id}
{}

EventRecord::EventRecord(EventRecord *record) : Record{record->ID}
{}

std::shared_ptr<EventRecord> EventRecord::getCopy()
{
    return std::make_shared<EventRecord>(*this);
}

bool EventRecord::isValid() const
{
    return Record::isValid();
}

auto SingleEventRecord::isValid() const -> bool
{
    return EventInfo::isValid();
}
