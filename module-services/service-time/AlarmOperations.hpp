// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AlarmRepository.hpp"
#include "SnoozedAlarmEventRecord.hpp"

#include <service-time/AlarmHandlerFactory.hpp>
#include <service-time/AlarmStatus.hpp>

#include <module-db/Interface/AlarmEventRecord.hpp>

#include <Service/Service.hpp>

namespace alarms
{
    template <typename T>
    using EventsContainer = std::vector<std::unique_ptr<T>>;

    class IAlarmOperations
    {
      public:
        using OnGetAlarmProcessed           = std::function<void(AlarmEventRecord)>;
        using OnGetAlarmWithStatusProcessed = std::function<void(std::pair<AlarmEventRecord, AlarmStatus>)>;
        using OnGetAlarmsInRangeProcessed   = std::function<void(std::pair<std::vector<AlarmEventRecord>, uint32_t>)>;
        using OnAddAlarmProcessed           = std::function<void(bool)>;
        using OnUpdateAlarmProcessed        = std::function<void(bool)>;
        using OnRemoveAlarmProcessed        = std::function<void(bool)>;
        using OnGetFirstNextSingleProcessed = std::function<void(SingleEventRecord)>;
        using OnGetAlarmsProcessed          = std::function<void(std::vector<SingleEventRecord>)>;
        using OnSnoozeRingingAlarm          = std::function<void(bool)>;
        using OnTurnOffRingingAlarm         = std::function<void(bool)>;
        using OnGetSnoozedAlarms            = std::function<void(std::vector<SingleEventRecord>)>;
        using OnSnoozedAlarmsCountChange    = std::function<void(unsigned)>;
        using OnActiveAlarmCountChange      = std::function<void(bool)>;
        using OnToggleAllProcessed          = std::function<void(bool)>;

        virtual ~IAlarmOperations() noexcept = default;

        using GetCurrentTime = std::function<TimePoint()>;

        virtual void updateEventsCache(TimePoint now) = 0;

        virtual void getAlarm(const std::uint32_t alarmId, OnGetAlarmProcessed callback)                     = 0;
        virtual void getAlarmWithStatus(const std::uint32_t alarmId, OnGetAlarmWithStatusProcessed callback) = 0;
        virtual void addAlarm(AlarmEventRecord record, OnAddAlarmProcessed callback)                         = 0;
        virtual void updateAlarm(AlarmEventRecord record, OnUpdateAlarmProcessed callback)                   = 0;
        virtual void removeAlarm(const std::uint32_t alarmId, OnRemoveAlarmProcessed callback)               = 0;
        virtual void getAlarmsInRange(std::uint32_t offset,
                                      std::uint32_t limit,
                                      OnGetAlarmsInRangeProcessed callback)                                  = 0;
        virtual void getNextSingleEvents(TimePoint start, OnGetAlarmsProcessed callback)                     = 0;
        virtual void turnOffRingingAlarm(const std::uint32_t id, OnTurnOffRingingAlarm callback)             = 0;
        virtual void turnOffSnoozedAlarm(const std::uint32_t id, OnTurnOffRingingAlarm callback)             = 0;
        virtual void snoozeRingingAlarm(const std::uint32_t id,
                                        const TimePoint nextAlarmTime,
                                        OnSnoozeRingingAlarm callback)                                       = 0;
        virtual void postponeSnooze(const std::uint32_t id,
                                    const TimePoint nextAlarmTime,
                                    OnSnoozeRingingAlarm callback)                                           = 0;
        virtual void minuteUpdated(TimePoint now)                                                            = 0;
        virtual void addAlarmExecutionHandler(const alarms::AlarmType type,
                                              const std::shared_ptr<alarms::AlarmHandler> handler)           = 0;
        virtual void stopAllSnoozedAlarms()                                                                  = 0;
        virtual void addSnoozedAlarmsCountChangeCallback(OnSnoozedAlarmsCountChange)                         = 0;
        virtual void addActiveAlarmCountChangeCallback(OnActiveAlarmCountChange)                             = 0;
        virtual void toggleAll(bool toggle, OnToggleAll callback)                                            = 0;
        virtual void getSnoozedAlarms(OnGetSnoozedAlarms callback)                                           = 0;
    };

    class IAlarmOperationsFactory
    {
      public:
        virtual ~IAlarmOperationsFactory() noexcept = default;

        virtual std::unique_ptr<IAlarmOperations> create(
            sys::Service *service,
            std::unique_ptr<AbstractAlarmEventsRepository> &&alarmEventsRepo,
            IAlarmOperations::GetCurrentTime getCurrentTimeCallback) const = 0;
    };

    class AlarmOperationsCommon : public IAlarmOperations
    {
      public:
        AlarmOperationsCommon(std::unique_ptr<AbstractAlarmEventsRepository> &&alarmEventsRepo,
                              GetCurrentTime getCurrentTimeCallback);

        void updateEventsCache(TimePoint now) override;

        void getAlarm(const std::uint32_t alarmId, OnGetAlarmProcessed callback) override;
        void getAlarmWithStatus(const std::uint32_t alarmId, OnGetAlarmWithStatusProcessed callback) override;
        void addAlarm(AlarmEventRecord record, OnAddAlarmProcessed callback) override;
        void updateAlarm(AlarmEventRecord record, OnUpdateAlarmProcessed callback) override;
        void removeAlarm(const std::uint32_t alarmId, OnRemoveAlarmProcessed callback) override;
        void getAlarmsInRange(std::uint32_t offset, std::uint32_t limit, OnGetAlarmsInRangeProcessed callback) override;
        void getNextSingleEvents(TimePoint start, OnGetAlarmsProcessed callback) override;
        void turnOffRingingAlarm(const std::uint32_t id, OnTurnOffRingingAlarm callback) override;
        void turnOffSnoozedAlarm(const std::uint32_t id, OnTurnOffRingingAlarm callback) override;
        void snoozeRingingAlarm(const std::uint32_t id,
                                const TimePoint nextAlarmTime,
                                OnSnoozeRingingAlarm callback) override;
        void postponeSnooze(const std::uint32_t id,
                            const TimePoint nextAlarmTime,
                            OnSnoozeRingingAlarm callback) override;
        void minuteUpdated(TimePoint now) override;
        void addAlarmExecutionHandler(const alarms::AlarmType type,
                                      const std::shared_ptr<alarms::AlarmHandler> handler) override;
        void stopAllSnoozedAlarms() override;
        void addSnoozedAlarmsCountChangeCallback(OnSnoozedAlarmsCountChange callback) override;
        void addActiveAlarmCountChangeCallback(OnActiveAlarmCountChange callback) override;
        void toggleAll(bool toggle, OnToggleAll callback) override;
        void getSnoozedAlarms(OnGetSnoozedAlarms callback) override;

      protected:
        std::unique_ptr<AbstractAlarmEventsRepository> alarmEventsRepo;
        AlarmHandlerFactory alarmHandlerFactory;

        // Events we are waiting for (on one timepoint)
        EventsContainer<SingleEventRecord> nextSingleEvents;
        EventsContainer<SingleEventRecord> ongoingSingleEvents;
        EventsContainer<SnoozedAlarmEventRecord> snoozedSingleEvents;

        alarms::AlarmType getAlarmEventType(const SingleEventRecord &event);
        virtual void handleAlarmEvent(const std::shared_ptr<AlarmEventRecord> &event,
                                      alarms::AlarmType alarmType,
                                      bool newStateOn);
        virtual bool turnOffAlarmIfFoundInBedtime(std::uint32_t id);
        bool turnOffAlarmIfFoundInOngoingEvents(std::uint32_t id);
        void switchAlarmExecution(const SingleEventRecord &singleAlarmEvent, bool newStateOn);

      private:
        GetCurrentTime getCurrentTimeCallback;
        OnSnoozedAlarmsCountChange onSnoozedAlarmsCountChangeCallback = nullptr;
        OnActiveAlarmCountChange onActiveAlarmCountChangeCallback     = nullptr;

        // Max 100 alarms for one minute seems reasonable, next events will be dropped
        constexpr static auto getNextSingleEventsOffset = 0;
        constexpr static auto getNextSingleEventsLimit  = 100;

        void onRepoGetFirstNextSingeResponse(OnGetFirstNextSingleProcessed handledCallback,
                                             TimePoint start,
                                             std::vector<AlarmEventRecord> records);
        void onRepoGetAlarmsResponse(TimePoint start,
                                     std::vector<AlarmEventRecord> records,
                                     OnGetAlarmsProcessed handledCallback);
        void checkAndUpdateCache(AlarmEventRecord record);
        void processEvents(TimePoint now);
        void processOngoingEvents();
        void processNextEventsQueue(const TimePoint now);
        void processSnoozedEventsQueue(const TimePoint now);
        virtual void onAlarmTurnedOff(const std::shared_ptr<AlarmEventRecord> &event, alarms::AlarmType alarmType);

        TimePoint getCurrentTime();
        void handleSnoozedAlarmsCountChange();
        void handleActiveAlarmsCountChange();
    };

    class CommonAlarmOperationsFactory : public IAlarmOperationsFactory
    {
      public:
        std::unique_ptr<IAlarmOperations> create(
            sys::Service *service,
            std::unique_ptr<AbstractAlarmEventsRepository> &&alarmEventsRepo,
            IAlarmOperations::GetCurrentTime getCurrentTimeCallback) const override;
    };

    auto findSingleEventById(EventsContainer<SingleEventRecord> &events, const std::uint32_t id)
        -> EventsContainer<SingleEventRecord>::iterator;
    auto findSnoozedEventById(EventsContainer<SnoozedAlarmEventRecord> &events, const std::uint32_t id)
        -> EventsContainer<SnoozedAlarmEventRecord>::iterator;
} // namespace alarms
