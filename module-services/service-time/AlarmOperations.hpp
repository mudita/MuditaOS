// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AlarmRepository.hpp"
#include "SnoozedAlarmEventRecord.hpp"

#include <service-time/AlarmHandlerFactory.hpp>

#include <module-db/Interface/AlarmEventRecord.hpp>

namespace alarms
{

    class IAlarmOperations
    {
      public:
        using OnGetAlarmProcessed         = std::function<void(AlarmEventRecord)>;
        using OnGetAlarmsInRangeProcessed = std::function<void(std::pair<std::vector<AlarmEventRecord>, uint32_t>)>;
        using OnAddAlarmProcessed         = std::function<void(bool)>;
        using OnUpdateAlarmProcessed      = std::function<void(bool)>;
        using OnRemoveAlarmProcessed      = std::function<void(bool)>;
        using OnGetNextSingleProcessed    = std::function<void(std::vector<SingleEventRecord>)>;
        using OnSnoozeRingingAlarm        = std::function<void(bool)>;
        using OnTurnOffRingingAlarm       = std::function<void(bool)>;

        virtual ~IAlarmOperations() noexcept = default;

        using GetCurrentTime = std::function<TimePoint()>;

        virtual void updateEventsCache(TimePoint now) = 0;

        virtual void getAlarm(const std::uint32_t alarmId, OnGetAlarmProcessed callback)       = 0;
        virtual void addAlarm(AlarmEventRecord record, OnAddAlarmProcessed callback)               = 0;
        virtual void updateAlarm(AlarmEventRecord record, OnUpdateAlarmProcessed callback)         = 0;
        virtual void removeAlarm(const std::uint32_t alarmId, OnRemoveAlarmProcessed callback)     = 0;
        virtual void getAlarmsInRange(TimePoint start,
                                      TimePoint end,
                                      std::uint32_t offset,
                                      std::uint32_t limit,
                                      OnGetAlarmsInRangeProcessed callback)                    = 0;
        virtual void getNextSingleEvents(TimePoint start, OnGetNextSingleProcessed callback)   = 0;
        virtual void turnOffRingingAlarm(const std::uint32_t id, OnTurnOffRingingAlarm callback)   = 0;
        virtual void snoozeRingingAlarm(const std::uint32_t id,
                                        const TimePoint nextAlarmTime,
                                        OnSnoozeRingingAlarm callback)                             = 0;
        virtual void minuteUpdated(TimePoint now)                                                  = 0;
        virtual void addAlarmExecutionHandler(const alarms::AlarmType type,
                                              const std::shared_ptr<alarms::AlarmHandler> handler) = 0;
    };

    class AlarmOperations : public IAlarmOperations
    {
      public:
        explicit AlarmOperations(std::unique_ptr<AbstractAlarmEventsRepository> &&alarmEventsRepo,
                                 GetCurrentTime getCurrentTimeCallback);

        void updateEventsCache(TimePoint now) override;

        void getAlarm(const std::uint32_t alarmId, OnGetAlarmProcessed callback) override;
        void addAlarm(AlarmEventRecord record, OnAddAlarmProcessed callback) override;
        void updateAlarm(AlarmEventRecord record, OnUpdateAlarmProcessed callback) override;
        void removeAlarm(const std::uint32_t alarmId, OnRemoveAlarmProcessed callback) override;
        void getAlarmsInRange(TimePoint start,
                              TimePoint end,
                              std::uint32_t offset,
                              std::uint32_t limit,
                              OnGetAlarmsInRangeProcessed callback) override;
        void getNextSingleEvents(TimePoint start, OnGetNextSingleProcessed callback) override;
        void turnOffRingingAlarm(const std::uint32_t id, OnTurnOffRingingAlarm callback) override;
        void snoozeRingingAlarm(const std::uint32_t id,
                                const TimePoint nextAlarmTime,
                                OnSnoozeRingingAlarm callback) override;
        void minuteUpdated(TimePoint now) override;
        void addAlarmExecutionHandler(const alarms::AlarmType type,
                                      const std::shared_ptr<alarms::AlarmHandler> handler) override;

      private:
        std::unique_ptr<AbstractAlarmEventsRepository> alarmEventsRepo;
        AlarmHandlerFactory alarmHandlerFactory;

        // Events we are waiting for (on one timepoint)
        std::vector<std::unique_ptr<SingleEventRecord>> nextSingleEvents;
        std::vector<std::unique_ptr<SingleEventRecord>> ongoingSingleEvents;
        std::vector<std::unique_ptr<SnoozedAlarmEventRecord>> snoozedSingleEvents;

        GetCurrentTime getCurrentTimeCallback;

        // Max 100 alarms for one minute seems reasonable, next events will be dropped
        constexpr static auto getNextSingleEventsOffset = 0;
        constexpr static auto getNextSingleEventsLimit  = 100;

        void onRepoGetNextResponse(OnGetNextSingleProcessed handledCallback,
                                   std::shared_ptr<std::vector<AlarmEventRecord>> nextEvents,
                                   TimePoint start,
                                   OnGetAlarmEventsRecurringInRange recurringCallback,
                                   std::vector<AlarmEventRecord> records);
        void onRepoGetRecurringInRangeResponse(OnGetNextSingleProcessed handledCallback,
                                               std::shared_ptr<std::vector<AlarmEventRecord>> nextEvents,
                                               TimePoint start,
                                               std::vector<AlarmEventRecord> records);

        void checkAndUpdateCache(AlarmEventRecord record);
        void switchAlarmExecution(const SingleEventRecord &singleAlarmEvent, bool newStateOn);
        void processNextEventsQueue(const TimePoint now);
        void processSnoozedEventsQueue(const TimePoint now);

        TimePoint getCurrentTime();
    };
} // namespace alarms
