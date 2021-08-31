// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AlarmRepository.hpp"

#include <module-db/Interface/AlarmEventRecord.hpp>

namespace alarms
{

    class IAlarmOperations
    {
      public:
        using OnGetAlarmProcessed         = std::function<void(AlarmEventRecord)>;
        using OnGetAlarmsInRangeProcessed = std::function<void(std::vector<AlarmEventRecord>)>;
        using OnAddAlarmProcessed         = std::function<void(bool)>;
        using OnUpdateAlarmProcessed      = std::function<void(bool)>;
        using OnRemoveAlarmProcessed      = std::function<void(bool)>;
        using OnGetNextSingleProcessed    = std::function<void(std::vector<SingleEventRecord>)>;

        virtual ~IAlarmOperations() noexcept = default;

        virtual void getAlarm(const std::uint32_t alarmId, OnGetAlarmProcessed callback)       = 0;
        virtual void addAlarm(AlarmEventRecord record, OnAddAlarmProcessed callback)           = 0;
        virtual void updateAlarm(AlarmEventRecord record, OnUpdateAlarmProcessed callback)     = 0;
        virtual void removeAlarm(const std::uint32_t alarmId, OnRemoveAlarmProcessed callback) = 0;
        virtual void getAlarmsInRange(TimePoint start,
                                      TimePoint end,
                                      std::uint32_t offset,
                                      std::uint32_t limit,
                                      OnGetAlarmsInRangeProcessed callback)                    = 0;
        virtual void getNextSingleEvents(TimePoint start, OnGetNextSingleProcessed callback)   = 0;
    };

    class AlarmOperations : public IAlarmOperations
    {
      public:
        explicit AlarmOperations(std::unique_ptr<AbstractAlarmEventsRepository> &&alarmEventsRepo);

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

      private:
        std::unique_ptr<AbstractAlarmEventsRepository> alarmEventsRepo;

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
    };
} // namespace alarms
