// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
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
        OnGetAlarmsProcessed callback = [&](std::vector<SingleEventRecord> singleEvents) {
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

    void AlarmOperationsCommon::getAlarmWithStatus(const std::uint32_t alarmId, OnGetAlarmWithStatusProcessed callback)
    {
        OnGetAlarmEventCallback repoCallback = [this, callback](AlarmEventRecord record) {
            // Check if snoozed
            auto foundSnoozed = findSnoozedEventById(snoozedSingleEvents, record.ID);
            if (foundSnoozed != snoozedSingleEvents.end()) {
                callback(std::pair<AlarmEventRecord, AlarmStatus>(record, AlarmStatus::Snoozed));
                return;
            }

            // Check if ringing or queued
            auto foundOngoing = findSingleEventById(ongoingSingleEvents, record.ID);
            if (foundOngoing != ongoingSingleEvents.end()) {
                if (foundOngoing == ongoingSingleEvents.begin()) {
                    callback(std::pair<AlarmEventRecord, AlarmStatus>(record, AlarmStatus::Ringing));
                }
                else {
                    callback(std::pair<AlarmEventRecord, AlarmStatus>(record, AlarmStatus::Queued));
                }
                return;
            }

            if (record.enabled) {
                callback(std::pair<AlarmEventRecord, AlarmStatus>(record, AlarmStatus::Activated));
            }
            else {
                callback(std::pair<AlarmEventRecord, AlarmStatus>(record, AlarmStatus::Deactivated));
            }
        };
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
            auto found = findSingleEventById(nextSingleEvents, record.ID);
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
            auto found = findSingleEventById(nextSingleEvents, alarmId);
            if (found != std::end(nextSingleEvents) && nextSingleEvents.size() == 1) {
                updateEventsCache(getCurrentTime());
            }
            callback(success);
        };
        alarmEventsRepo->removeAlarmEvent(alarmId, repoCallback);
    }

    void AlarmOperationsCommon::getAlarmsInRange(std::uint32_t offset,
                                                 std::uint32_t limit,
                                                 OnGetAlarmsInRangeProcessed callback)
    {
        OnGetAlarmEventsInRangeCallback repoCallback = [callback](auto vals) { callback(std::move(vals)); };
        alarmEventsRepo->getAlarmEventsInRange(offset, limit, repoCallback);
    }

    void AlarmOperationsCommon::getNextSingleEvents(TimePoint start, OnGetAlarmsProcessed callback)
    {
        OnGetAlarmEventsCallback repoGetAlarmsCallback = [&, callback, start](std::vector<AlarmEventRecord> records) {
            onRepoGetAlarmsResponse(start, records, callback);
        };

        alarmEventsRepo->getAlarmEnabledEvents(repoGetAlarmsCallback);
    }

    void AlarmOperationsCommon::turnOffRingingAlarm(const std::uint32_t id, OnTurnOffRingingAlarm callback)
    {
        auto found = findSingleEventById(ongoingSingleEvents, id);
        if (found == ongoingSingleEvents.end()) {
            LOG_ERROR("Trying to turn off nonexisting event");
            callback(false);
            return;
        }
        switchAlarmExecution(*(*found), false);
        ongoingSingleEvents.erase(found);

        processOngoingEvents();

        handleActiveAlarmsCountChange();
        callback(true);
    }

    void AlarmOperationsCommon::turnOffSnoozedAlarm(const std::uint32_t id, OnTurnOffRingingAlarm callback)
    {
        auto found = findSnoozedEventById(snoozedSingleEvents, id);
        if (found == snoozedSingleEvents.end()) {
            LOG_ERROR("Trying to turn off nonexisting event");
            callback(false);
            return;
        }
        snoozedSingleEvents.erase(found);
        handleSnoozedAlarmsCountChange();
        handleActiveAlarmsCountChange();
        callback(true);
    }

    void AlarmOperationsCommon::snoozeRingingAlarm(const std::uint32_t id,
                                                   const TimePoint nextAlarmTime,
                                                   OnSnoozeRingingAlarm callback)
    {
        auto found = findSingleEventById(ongoingSingleEvents, id);
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

        // if ID already in snoozed -> remove
        for (auto it = snoozedSingleEvents.begin(); it != snoozedSingleEvents.end();) {
            if ((*it)->parent->ID == id) {
                it = snoozedSingleEvents.erase(it);
            }
            else {
                ++it;
            }
        }

        newSnoozed->startDate = nextAlarmTime;
        snoozedSingleEvents.push_back(std::move(newSnoozed));

        switchAlarmExecution(*(*found), false);
        ongoingSingleEvents.erase(found);

        processOngoingEvents();

        handleSnoozedAlarmsCountChange();
        handleActiveAlarmsCountChange();

        callback(true);
    }

    void AlarmOperationsCommon::postponeSnooze(const std::uint32_t id,
                                               const TimePoint nextAlarmTime,
                                               OnSnoozeRingingAlarm callback)
    {
        auto found = findSnoozedEventById(snoozedSingleEvents, id);
        if (found == snoozedSingleEvents.end()) {
            LOG_ERROR("Trying to postpone nonexisting snooze");
            callback(false);
            return;
        }
        (*found)->startDate = nextAlarmTime;

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

    void AlarmOperationsCommon::onRepoGetAlarmsResponse(TimePoint start,
                                                        std::vector<AlarmEventRecord> records,
                                                        OnGetAlarmsProcessed handledCallback)
    {
        std::vector<SingleEventRecord> outEvents;
        for (auto &record : records) {
            if (outEvents.empty()) {
                outEvents.push_back(record.getNextSingleEvent(start));
            }
            else {
                auto event = record.getNextSingleEvent(start);
                if (event.startDate != TIME_POINT_INVALID) {
                    if (event.startDate < outEvents.front().startDate) {
                        outEvents.clear();
                        outEvents.push_back(event);
                    }
                    else if (event.startDate == outEvents.front().startDate) {
                        outEvents.push_back(event);
                    }
                }
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

        if (!isHandlingInProgress) {
            processOngoingEvents();
        }
    }

    void AlarmOperationsCommon::processOngoingEvents()
    {
        if (!ongoingSingleEvents.empty()) {
            switchAlarmExecution(*(ongoingSingleEvents.front()), true);
            handleActiveAlarmsCountChange();
            handleSnoozedAlarmsCountChange();
        }
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
                onAlarmTurnedOff(event, alarmType);
            }
        }
    }

    void AlarmOperationsCommon::onAlarmTurnedOff(const std::shared_ptr<AlarmEventRecord> &event,
                                                 alarms::AlarmType alarmType)
    {}

    auto AlarmOperationsCommon::processNextEventsQueue(const TimePoint now) -> void
    {
        if (nextSingleEvents.front()->startDate <= now) {
            if (!isCriticalBatteryLevel) {
                triggerAlarm();
            }
            nextSingleEvents.clear();
            updateEventsCache(now);
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
    }

    auto AlarmOperationsCommon::stopAllSnoozedAlarms() -> void
    {
        snoozedSingleEvents.clear();
        handleSnoozedAlarmsCountChange();
        handleActiveAlarmsCountChange();
    }

    auto AlarmOperationsCommon::stopAllRingingAlarms() -> void
    {
        if (!ongoingSingleEvents.empty()) {
            for (auto &event : ongoingSingleEvents) {
                switchAlarmExecution(*event, false);
            }
            ongoingSingleEvents.clear();
            handleActiveAlarmsCountChange();
        }
    }

    void AlarmOperationsCommon::triggerAlarm()
    {
        ongoingSingleEvents.insert(ongoingSingleEvents.end(),
                                   std::make_move_iterator(nextSingleEvents.begin()),
                                   std::make_move_iterator(nextSingleEvents.end()));
    }

    void AlarmOperationsCommon::toggleAll(bool toggle, OnToggleAll callback)
    {
        OnToggleAll repoCallback = [&, callback](bool success) mutable {
            updateEventsCache(getCurrentTime());

            callback(success);
        };

        alarmEventsRepo->toggleAll(toggle, repoCallback);
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

    void AlarmOperationsCommon::getSnoozedAlarms(OnGetSnoozedAlarms callback)
    {
        std::vector<SingleEventRecord> snoozedEvents;
        if (!snoozedSingleEvents.empty()) {
            for (const auto &evt : snoozedSingleEvents) {
                snoozedEvents.push_back(*evt.get());
            }

            auto isEarlier = [](const SingleEventRecord &a, const SingleEventRecord &b) {
                return a.startDate < b.startDate;
            };
            std::sort(snoozedEvents.begin(), snoozedEvents.end(), isEarlier);
        }
        callback(std::move(snoozedEvents));
    }

    void AlarmOperationsCommon::handleCriticalBatteryLevel()
    {
        isCriticalBatteryLevel = true;
        stopAllRingingAlarms();
        stopAllSnoozedAlarms();
    }

    void AlarmOperationsCommon::handleNormalBatteryLevel()
    {
        isCriticalBatteryLevel = false;
    }

    auto findSingleEventById(std::vector<std::unique_ptr<SingleEventRecord>> &events, const std::uint32_t id)
        -> std::vector<std::unique_ptr<SingleEventRecord>>::iterator
    {
        return std::find_if(events.begin(), events.end(), [id](const std::unique_ptr<SingleEventRecord> &event) {
            return id == event->parent->ID;
        });
    }
    auto findSnoozedEventById(std::vector<std::unique_ptr<SnoozedAlarmEventRecord>> &events, const std::uint32_t id)
        -> std::vector<std::unique_ptr<SnoozedAlarmEventRecord>>::iterator
    {
        return std::find_if(events.begin(), events.end(), [id](const std::unique_ptr<SnoozedAlarmEventRecord> &event) {
            return id == event->parent->ID;
        });
    }
} // namespace alarms
