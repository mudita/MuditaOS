// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EventRecord.hpp"

#include <rrule/rrule.hpp>

bool EventInfo::isValid() const
{
    return !(startDate == TIME_POINT_INVALID || startDate == TIME_POINT_MAX || endDate == TIME_POINT_INVALID);
}

EventRecord::EventRecord(
    uint32_t id, const UTF8 &name, TimePoint startDate, uint32_t duration, bool isAllDay, const std::string &rruleText)
    : Record{id}, EventInfo{name, startDate, TIME_POINT_INVALID, duration, isAllDay}, rruleText{rruleText}
{
    if (rruleText.empty()) {
        this->endDate = startDate + std::chrono::minutes(duration);
    }
    else {
        rrule::RRule recurrence(rruleText);

        if (recurrence.until == TIME_POINT_INVALID && recurrence.count == 0) {
            this->endDate = TIME_POINT_MAX;
        }
        else {
            this->endDate = recurrence.generateLastTimePoint(startDate) + std::chrono::minutes(duration);
        }
    }
}

EventRecord::EventRecord(EventRecord *record)
    : Record{record->ID}, EventInfo{record->name,
                                    record->startDate,
                                    record->endDate,
                                    record->duration,
                                    record->isAllDay},
      rruleText{record->rruleText}
{}

std::vector<SingleEventRecord> EventRecord::generateSingleEvents(TimePoint from, TimePoint to, uint32_t count)
{
    std::vector<SingleEventRecord> singleEvents;
    auto parentEvent = std::make_shared<EventRecord>(this);

    if (rruleText.empty()) {
        if (startDate > from && startDate < to) {
            singleEvents.emplace_back(SingleEventRecord(parentEvent, startDate, endDate));
        }
    }
    else {
        if (startDate > to) {
            return {};
        }
        rrule::RRule recurence(rruleText);

        auto timepoints = recurence.generateEventTimePoints(startDate, from, to, count);
        for (auto timepoint : timepoints) {
            singleEvents.emplace_back(parentEvent, timepoint, timepoint + std::chrono::minutes(duration));
        }
    }
    return singleEvents;
}

std::shared_ptr<EventRecord> EventRecord::getCopy()
{
    return std::make_shared<EventRecord>(*this);
}

SingleEventRecord EventRecord::getNextSingleEvent(TimePoint from)
{
    auto parentEvent = getCopy();

    if (rruleText.empty()) {
        if (startDate < from) {
            return SingleEventRecord();
        }
        return SingleEventRecord(parentEvent, startDate, endDate);
    }
    else {
        rrule::RRule recurence(rruleText);

        const TimePoint timepoint = recurence.generateNextTimePoint(startDate, from);

        if (timepoint == TIME_POINT_INVALID) {
            return SingleEventRecord();
        }
        return SingleEventRecord(parentEvent, timepoint, timepoint + std::chrono::minutes(duration));
    }
}

bool EventRecord::isValid() const
{
    return (EventInfo::isValid() && Record::isValid());
}
