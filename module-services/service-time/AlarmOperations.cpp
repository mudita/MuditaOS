// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmOperations.hpp"

#include <module-db/Interface/AlarmEventRecord.hpp>
#include <module-db/Interface/EventRecord.hpp>

namespace alarms
{
    AlarmOperations::AlarmOperations(std::unique_ptr<AbstractAlarmEventsRepository> &&alarmEventsRepo,
                                     GetCurrentTime getCurrentTimeCallback)
        : alarmEventsRepo{std::move(alarmEventsRepo)}, getCurrentTimeCallback{getCurrentTimeCallback} {};

    void AlarmOperations::updateEventsCache(TimePoint now)
    {
        OnGetNextSingleProcessed callback = [&](std::vector<SingleEventRecord> singleEvents) {
            nextSingleEvents = std::move(singleEvents);
        };
        getNextSingleEvents(now, callback);
    }

    void AlarmOperations::getAlarm(const std::uint32_t alarmId, OnGetAlarmProcessed callback)
    {
        OnGetAlarmEventCallback repoCallback = [callback](AlarmEventRecord record) { callback(record); };
        alarmEventsRepo->getAlarmEvent(alarmId, repoCallback);
    }

    void AlarmOperations::addAlarm(AlarmEventRecord record, OnAddAlarmProcessed callback)
    {
        OnAddAlarmEventCallback repoCallback = [&, callback, record](bool success) mutable {
            checkAndUpdateCache(record);
            callback(success);
        };
        alarmEventsRepo->addAlarmEvent(record, repoCallback);
    }

    void AlarmOperations::updateAlarm(AlarmEventRecord record, OnUpdateAlarmProcessed callback)
    {
        OnUpdateAlarmEventCallback repoCallback = [&, callback, record](bool success) mutable {
            auto found =
                std::find_if(nextSingleEvents.begin(),
                             nextSingleEvents.end(),
                             [recordId = record.ID](const SingleEventRecord &e) { return e.parent->ID == recordId; });

            if (found != std::end(nextSingleEvents)) {
                updateEventsCache(getCurrentTime());
            }
            else {
                checkAndUpdateCache(record);
            }
            callback(success);
        };
        alarmEventsRepo->updateAlarmEvent(record, repoCallback);
    }

    void AlarmOperations::removeAlarm(const std::uint32_t alarmId, OnRemoveAlarmProcessed callback)
    {
        OnRemoveAlarmEventCallback repoCallback = [&, callback, alarmId](bool success) {
            auto found = std::find_if(nextSingleEvents.begin(),
                                      nextSingleEvents.end(),
                                      [alarmId](const SingleEventRecord &e) { return e.parent->ID == alarmId; });

            if (found != std::end(nextSingleEvents) && nextSingleEvents.size() == 1) {
                updateEventsCache(getCurrentTime());
            }
            callback(success);
        };
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

    void AlarmOperations::checkAndUpdateCache(AlarmEventRecord record)
    {
        auto nearestNewSingleEvent = record.getNextSingleEvent(getCurrentTime());

        if (nearestNewSingleEvent.EventInfo::isValid() && nearestNewSingleEvent.startDate > getCurrentTime()) {
            auto alarmEvent = std::dynamic_pointer_cast<AlarmEventRecord>(nearestNewSingleEvent.parent);
            if (record.enabled &&
                (nextSingleEvents.empty() || nearestNewSingleEvent.startDate <= nextSingleEvents.front().startDate)) {
                updateEventsCache(getCurrentTime());
            }
        }
    }

    auto AlarmOperations::minuteUpdated(TimePoint now) -> void
    {
        if (nextSingleEvents.empty() || nextSingleEvents.front().startDate > now) {
            return;
        }

        executeAlarm(nextSingleEvents.front());
    }

    void AlarmOperations::addAlarmExecutionHandler(const alarms::AlarmType type,
                                                   const std::shared_ptr<alarms::AlarmHandler> handler)
    {
        alarmHandlerFactory.addHandler(type, handler);
    }

    void AlarmOperations::executeAlarm(const SingleEventRecord &singleAlarmEvent)
    {
        alarms::AlarmType alarmType = alarms::AlarmType::None;
        if (typeid(*(singleAlarmEvent.parent)) == typeid(AlarmEventRecord)) {
            alarmType = alarms::AlarmType::Clock;
        }

        auto alarmEventPtr = std::dynamic_pointer_cast<AlarmEventRecord>(singleAlarmEvent.parent);
        if (alarmEventPtr) {
            auto handler = alarmHandlerFactory.getHandler(alarmType);
            if (handler) {
                handler->handle(*alarmEventPtr);
            }
        }
        else {
            LOG_WARN("Parent type is not AlarmEventRecord!");
        }
    }

    TimePoint AlarmOperations::getCurrentTime()
    {
        if (!getCurrentTimeCallback) {
            return TIME_POINT_INVALID;
        }
        return getCurrentTimeCallback();
    }
} // namespace alarms
