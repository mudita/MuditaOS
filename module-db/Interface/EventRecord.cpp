// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EventRecord.hpp"

#include <rrule/rrule.hpp>

EventRecord::EventRecord(
    uint32_t id, UTF8 name, TimePoint startDate, TimePoint endDate, uint32_t duration, bool isAllDay, std::string rrule)
    : Record{id}, EventInfo{name, startDate, endDate, duration, isAllDay}, rrule{rrule}
{
    if (rrule.size() == 0) {
        this->endDate = startDate + std::chrono::minutes(duration);
    }
    else {
        rrule::RRule recurrence;
        recurrence.parseFromString(rrule);

        if (recurrence.until == TIME_POINT_INVALID && recurrence.count == 0) {
            this->endDate = TIME_POINT_MAX;
        }
        else {
            this->endDate = recurrence.generateLastTimePoint(startDate) + std::chrono::minutes(duration);
        }
    }
}

EventRecord::EventRecord(EventRecord *er)
    : Record{er->ID}, EventInfo{er->name, er->startDate, er->endDate, er->duration, er->isAllDay}, rrule{er->rrule}
{}

std::vector<SingleEventRecord> EventRecord::generateSingleEvents(TimePoint from, TimePoint to, uint32_t count)
{
    std::vector<SingleEventRecord> singleEvents;
    auto parentEvent = std::shared_ptr<EventRecord>(new EventRecord(this));

    if (rrule.size() == 0) {
        if (startDate > from && startDate < to) {
            singleEvents.push_back(SingleEventRecord(parentEvent, startDate, endDate));
        }
    }
    else {
        if (startDate > to) {
            return {};
        }
        rrule::RRule recurence;
        recurence.parseFromString(rrule);

        auto timepoints = recurence.generateEventTimePoints(startDate, from, to, count);
        for (auto timepoint : timepoints) {
            singleEvents.push_back(
                SingleEventRecord(parentEvent, timepoint, timepoint + std::chrono::minutes(duration)));
        }
    }
    return singleEvents;
}

SingleEventRecord EventRecord::getNextSingleEvent(TimePoint from)
{
    auto parentEvent = std::shared_ptr<EventRecord>(new EventRecord(this));

    if (rrule.size() == 0) {
        if (startDate >= from) {
            return SingleEventRecord(parentEvent, startDate, endDate);
        }
        else {
            return SingleEventRecord();
        }
    }
    else {
        rrule::RRule recurence;
        recurence.parseFromString(rrule);

        TimePoint timepoint = recurence.generateNextTimePoint(startDate, from);

        if (timepoint != TIME_POINT_INVALID) {
            return SingleEventRecord(parentEvent, timepoint, timepoint + std::chrono::minutes(duration));
        }
        else {
            return SingleEventRecord();
        }
    }
}
