// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmOperations.hpp"

#include <module-db/Interface/AlarmEventRecord.hpp>
#include <module-db/Interface/EventRecord.hpp>

#include <vector>

namespace alarms
{
    std::unique_ptr<IAlarmOperations> CommonAlarmOperationsFactory::create(
        [[maybe_unused]] sys::Service *service,
        std::unique_ptr<AbstractAlarmEventsRepository> &&alarmEventsRepo,
        IAlarmOperations::GetCurrentTime getCurrentTimeCallback) const
    {
        return std::make_unique<AlarmOperationsCommon>(std::move(alarmEventsRepo), getCurrentTimeCallback);
    }

    AlarmOperationsCommon::AlarmOperationsCommon(std::unique_ptr<AbstractAlarmEventsRepository> &&alarmEventsRepo,
                                                 GetCurrentTime getCurrentTimeCallback)
        : alarmEventsRepo{std::move(alarmEventsRepo)}, getCurrentTimeCallback{getCurrentTimeCallback} {};

    void AlarmOperationsCommon::updateEventsCache(TimePoint now)
    {
        OnGetNextSingleProcessed callback = [&](std::vector<SingleEventRecord> singleEvents) {
            nextSingleEvents.clear();
            nextSingleEvents.reserve(singleEvents.size());
            for (auto &ev : singleEvents) {
                nextSingleEvents.emplace_back(std::make_unique<SingleEventRecord>(ev));
            }
            handleActiveAlarmsCountChange();
        };
        getNextSingleEvents(now, callback);
    }

    void AlarmOperationsCommon::getAlarm(const std::uint32_t alarmId, OnGetAlarmProcessed callback)
    {
        OnGetAlarmEventCallback repoCallback = [callback](AlarmEventRecord record) { callback(record); };
        alarmEventsRepo->getAlarmEvent(alarmId, repoCallback);
    }

    void AlarmOperationsCommon::addAlarm(AlarmEventRecord record, OnAddAlarmProcessed callback)
    {
        OnAddAlarmEventCallback repoCallback = [&, callback, record](bool success) mutable {
            checkAndUpdateCache(record);
            callback(success);
        };
        alarmEventsRepo->addAlarmEvent(record, repoCallback);
    }

    void AlarmOperationsCommon::updateAlarm(AlarmEventRecord record, OnUpdateAlarmProcessed callback)
    {
        OnUpdateAlarmEventCallback repoCallback = [&, callback, record](bool success) mutable {
            auto found = std::find_if(nextSingleEvents.begin(),
                                      nextSingleEvents.end(),
                                      [recordId = record.ID](const std::unique_ptr<SingleEventRecord> &e) {
                                          return e->parent->ID == recordId;
                                      });

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

    void AlarmOperationsCommon::removeAlarm(const std::uint32_t alarmId, OnRemoveAlarmProcessed callback)
    {
        OnRemoveAlarmEventCallback repoCallback = [&, callback, alarmId](bool success) {
            auto found = std::find_if(
                nextSingleEvents.begin(),
                nextSingleEvents.end(),
                [alarmId](const std::unique_ptr<SingleEventRecord> &e) { return e->parent->ID == alarmId; });

            if (found != std::end(nextSingleEvents) && nextSingleEvents.size() == 1) {
                updateEventsCache(getCurrentTime());
            }
            callback(success);
        };
        alarmEventsRepo->removeAlarmEvent(alarmId, repoCallback);
    }

    void AlarmOperationsCommon::getAlarmsInRange(
        TimePoint start, TimePoint end, std::uint32_t offset, std::uint32_t limit, OnGetAlarmsInRangeProcessed callback)
    {
        OnGetAlarmEventsInRangeCallback repoCallback = [callback](auto vals) { callback(std::move(vals)); };
        alarmEventsRepo->getAlarmEventsInRange(start, end, offset, limit, repoCallback);
    }

    void AlarmOperationsCommon::getFirstNextSingleEvent(TimePoint start, OnGetFirstNextSingleProcessed callback)
    {
        OnGetNextCallback repoGetFirstNextCallback = [&, callback, start](std::vector<AlarmEventRecord> records) {
            onRepoGetFirstNextSingeResponse(callback, start, records);
        };

        alarmEventsRepo->getNext(start, getNextSingleEventsOffset, getNextSingleEventsLimit, repoGetFirstNextCallback);
    }

    void AlarmOperationsCommon::getNextSingleEvents(TimePoint start, OnGetNextSingleProcessed callback)
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

    void AlarmOperationsCommon::turnOffRingingAlarm(const std::uint32_t id, OnTurnOffRingingAlarm callback)
    {
        auto found =
            std::find_if(ongoingSingleEvents.begin(),
                         ongoingSingleEvents.end(),
                         [id](const std::unique_ptr<SingleEventRecord> &event) { return id == event->parent->ID; });
        if (found == ongoingSingleEvents.end()) {
            LOG_ERROR("Trying to turn off nonexisting event");
            callback(false);
            return;
        }
        switchAlarmExecution(*(*found), false);
        ongoingSingleEvents.erase(found);
        callback(true);
    }

    void AlarmOperationsCommon::snoozeRingingAlarm(const std::uint32_t id,
                                                   const TimePoint nextAlarmTime,
                                                   OnSnoozeRingingAlarm callback)
    {
        auto found =
            std::find_if(ongoingSingleEvents.begin(),
                         ongoingSingleEvents.end(),
                         [id](const std::unique_ptr<SingleEventRecord> &event) { return id == event->parent->ID; });
        if (found == ongoingSingleEvents.end()) {
            LOG_ERROR("Trying to snooze nonexisting event");
            callback(false);
            return;
        }

        auto newSnoozed = std::make_unique<SnoozedAlarmEventRecord>((*found).get());

        if (typeid(*(*found)) == typeid(SnoozedAlarmEventRecord)) {
            auto oldSnoozedPtr      = dynamic_cast<SnoozedAlarmEventRecord *>((*found).get());
            newSnoozed->snoozeCount = oldSnoozedPtr->snoozeCount + 1;
        }
        else if (typeid(*(*found)) != typeid(SingleEventRecord)) {
            LOG_ERROR("Unkown alarm type detected");
            callback(false);
            return;
        }

        newSnoozed->startDate = nextAlarmTime;
        snoozedSingleEvents.push_back(std::move(newSnoozed));

        switchAlarmExecution(*(*found), false);
        ongoingSingleEvents.erase(found);

        handleSnoozedAlarmsCountChange();
        handleActiveAlarmsCountChange();

        callback(true);
    }

    void AlarmOperationsCommon::onRepoGetFirstNextSingeResponse(OnGetFirstNextSingleProcessed handledCallback,
                                                                TimePoint start,
                                                                std::vector<AlarmEventRecord> records)
    {
        if (records.empty()) {
            handledCallback({});
        }
        else {
            handledCallback(records.front().getNextSingleEvent(start));
        }
    }

    void AlarmOperationsCommon::onRepoGetNextResponse(OnGetNextSingleProcessed handledCallback,
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

    void AlarmOperationsCommon::onRepoGetRecurringInRangeResponse(
        OnGetNextSingleProcessed handledCallback,
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

    void AlarmOperationsCommon::checkAndUpdateCache(AlarmEventRecord record)
    {
        auto nearestNewSingleEvent = record.getNextSingleEvent(getCurrentTime());

        if (nearestNewSingleEvent.EventInfo::isValid() && nearestNewSingleEvent.startDate > getCurrentTime()) {
            auto alarmEvent = std::dynamic_pointer_cast<AlarmEventRecord>(nearestNewSingleEvent.parent);
            if (record.enabled &&
                (nextSingleEvents.empty() || nearestNewSingleEvent.startDate <= nextSingleEvents.front()->startDate)) {
                updateEventsCache(getCurrentTime());
            }
        }
    }

    auto AlarmOperationsCommon::minuteUpdated(TimePoint now) -> void
    {
        processEvents(now);
    }

    void AlarmOperationsCommon::processEvents(TimePoint now)
    {
        const auto isHandlingInProgress = !ongoingSingleEvents.empty();
        if (!nextSingleEvents.empty()) {
            processNextEventsQueue(now);
        }

        if (!snoozedSingleEvents.empty()) {
            processSnoozedEventsQueue(now);
        }

        if (!isHandlingInProgress && !ongoingSingleEvents.empty()) {
            switchAlarmExecution(*(ongoingSingleEvents.front()), true);
        }
        handleActiveAlarmsCountChange();
    }

    void AlarmOperationsCommon::addAlarmExecutionHandler(const alarms::AlarmType type,
                                                         const std::shared_ptr<alarms::AlarmHandler> handler)
    {
        alarmHandlerFactory.addHandler(type, handler);
    }

    void AlarmOperationsCommon::switchAlarmExecution(const SingleEventRecord &singleAlarmEvent, bool newStateOn)
    {
        if (auto alarmEventPtr = std::dynamic_pointer_cast<AlarmEventRecord>(singleAlarmEvent.parent); alarmEventPtr) {
            handleAlarmEvent(alarmEventPtr, getAlarmEventType(singleAlarmEvent), newStateOn);
        }
        else {
            LOG_WARN("Parent type is not AlarmEventRecord!");
        }
    }

    alarms::AlarmType AlarmOperationsCommon::getAlarmEventType(const SingleEventRecord &event)
    {
        if (typeid(*(event.parent)) == typeid(AlarmEventRecord)) {
            return alarms::AlarmType::Clock;
        }
        return alarms::AlarmType::None;
    }

    void AlarmOperationsCommon::handleAlarmEvent(const std::shared_ptr<AlarmEventRecord> &event,
                                                 alarms::AlarmType alarmType,
                                                 bool newStateOn)
    {
        if (auto handler = alarmHandlerFactory.getHandler(alarmType); handler) {
            if (newStateOn) {
                handler->handle(*event);
            }
            else {
                handler->handleOff(*event);
            }
        }
    }

    auto AlarmOperationsCommon::processNextEventsQueue(const TimePoint now) -> void
    {
        if (nextSingleEvents.front()->startDate <= now) {
            ongoingSingleEvents.insert(ongoingSingleEvents.end(),
                                       std::make_move_iterator(nextSingleEvents.begin()),
                                       std::make_move_iterator(nextSingleEvents.end()));
            nextSingleEvents.clear();
        }
    }

    auto AlarmOperationsCommon::processSnoozedEventsQueue(const TimePoint now) -> void
    {
        for (auto it = snoozedSingleEvents.begin(); it != snoozedSingleEvents.end();) {
            if ((*it)->startDate <= now) {
                ongoingSingleEvents.push_back(std::move(*it));
                it = snoozedSingleEvents.erase(it);
            }
            else {
                ++it;
            }
        }
        handleSnoozedAlarmsCountChange();
    }

    auto AlarmOperationsCommon::stopAllSnoozedAlarms() -> void
    {
        snoozedSingleEvents.clear();
        handleSnoozedAlarmsCountChange();
        handleActiveAlarmsCountChange();
    }

    auto AlarmOperationsCommon::addSnoozedAlarmsCountChangeCallback(OnSnoozedAlarmsCountChange callback) -> void
    {
        onSnoozedAlarmsCountChangeCallback = callback;
    }

    auto AlarmOperationsCommon::addActiveAlarmCountChangeCallback(OnActiveAlarmCountChange callback) -> void
    {
        onActiveAlarmCountChangeCallback = callback;
    }

    TimePoint AlarmOperationsCommon::getCurrentTime()
    {
        if (!getCurrentTimeCallback) {
            return TIME_POINT_INVALID;
        }
        return getCurrentTimeCallback();
    }

    void AlarmOperationsCommon::handleSnoozedAlarmsCountChange()
    {
        if (onSnoozedAlarmsCountChangeCallback) {
            onSnoozedAlarmsCountChangeCallback(snoozedSingleEvents.size());
        }
    }

    void AlarmOperationsCommon::handleActiveAlarmsCountChange()
    {
        if (onActiveAlarmCountChangeCallback) {
            onActiveAlarmCountChangeCallback(!nextSingleEvents.empty() || !snoozedSingleEvents.empty() ||
                                             !ongoingSingleEvents.empty());
        }
    }

} // namespace alarms
