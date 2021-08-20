// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmOperations.hpp"

namespace alarms
{
    AlarmOperations::AlarmOperations(std::unique_ptr<AbstractAlarmEventsRepository> &&alarmEventsRepo)
        : alarmEventsRepo{std::move(alarmEventsRepo)} {};

    void AlarmOperations::getAlarm(const std::uint32_t alarmId, OnGetAlarmProcessed callback)
    {
        OnGetAlarmEventCallback repoCallback = [callback](AlarmEventRecord record) { callback(record); };
        alarmEventsRepo->getAlarmEvent(alarmId, repoCallback);
    }

    void AlarmOperations::addAlarm(AlarmEventRecord record, OnAddAlarmProcessed callback)
    {
        OnAddAlarmEventCallback repoCallback = [callback](bool success) { callback(success); };
        alarmEventsRepo->addAlarmEvent(record, repoCallback);
    }
    void AlarmOperations::updateAlarm(AlarmEventRecord record, OnUpdateAlarmProcessed callback)
    {
        OnUpdateAlarmEventCallback repoCallback = [callback](bool success) { callback(success); };
        alarmEventsRepo->updateAlarmEvent(record, repoCallback);
    }

    void AlarmOperations::removeAlarm(const std::uint32_t alarmId, OnRemoveAlarmProcessed callback)
    {
        OnRemoveAlarmEventCallback repoCallback = [callback](bool success) { callback(success); };
        alarmEventsRepo->removeAlarmEvent(alarmId, repoCallback);
    }

    void AlarmOperations::getAlarmsInRange(
        TimePoint start, TimePoint end, std::uint32_t offset, std::uint32_t limit, OnGetAlarmsInRangeProcessed callback)
    {
        OnGetAlarmEventsInRangeCallback repoCallback = [callback](std::vector<AlarmEventRecord> records) {
            callback(records);
        };
        alarmEventsRepo->getAlarmEventsInRange(start, end, offset, limit, repoCallback);
    }

    void AlarmOperations::getNextSingleEvents(TimePoint start, OnGetNextSingleProcessed callback)
    {
        auto nextEvents = std::make_shared<std::vector<AlarmEventRecord>>();

        OnGetAlarmEventsRecurringInRange repoGetRecurringCallback =
            [&, callback, nextEvents, start](std::vector<AlarmEventRecord> records) {
                onRepoGetRecurringInRangeResponse(callback, nextEvents, start, records);
            };

        OnGetNextCallback repoGetNextCallback =
            [&, callback, nextEvents, start, repoGetRecurringCallback](std::vector<AlarmEventRecord> records) {
                onRepoGetNextResponse(callback, nextEvents, start, repoGetRecurringCallback, records);
            };

        alarmEventsRepo->getNext(start, getNextSingleEventsOffset, getNextSingleEventsLimit, repoGetNextCallback);
    }

    void AlarmOperations::onRepoGetNextResponse(OnGetNextSingleProcessed handledCallback,
                                                std::shared_ptr<std::vector<AlarmEventRecord>> nextEvents,
                                                TimePoint start,
                                                OnGetAlarmEventsRecurringInRange recurringCallback,
                                                std::vector<AlarmEventRecord> records)
    {
        if (records.empty()) {
            handledCallback({});
        }
        else {
            *nextEvents              = std::move(records);
            auto firstEventTimePoint = (*nextEvents)[0].startDate;
            nextEvents->erase(std::remove_if(nextEvents->begin(),
                                             nextEvents->end(),
                                             [](AlarmEventRecord rec) { return !rec.rruleText.empty(); }),
                              nextEvents->end());
            alarmEventsRepo->getAlarmEventsRecurringInRange(
                start, firstEventTimePoint, getNextSingleEventsOffset, getNextSingleEventsLimit, recurringCallback);
        }
    }

    void AlarmOperations::onRepoGetRecurringInRangeResponse(OnGetNextSingleProcessed handledCallback,
                                                            std::shared_ptr<std::vector<AlarmEventRecord>> nextEvents,
                                                            TimePoint start,
                                                            std::vector<AlarmEventRecord> records)
    {
        std::vector<SingleEventRecord> outEvents;
        if (!records.empty()) {
            (*nextEvents)
                .insert(nextEvents->end(),
                        std::make_move_iterator(records.begin()),
                        std::make_move_iterator(records.end()));
        }
        TimePoint nearestTimePoint = TIME_POINT_MAX;
        for (auto &event : (*nextEvents)) {
            auto newSingle = event.getNextSingleEvent(start);
            if (newSingle.startDate == nearestTimePoint) {
                outEvents.push_back(newSingle);
            }
            else if (newSingle.startDate < nearestTimePoint) {
                nearestTimePoint = newSingle.startDate;
                outEvents.clear();
                outEvents.push_back(newSingle);
            }
        }
        handledCallback(outEvents);
    }
} // namespace alarms
