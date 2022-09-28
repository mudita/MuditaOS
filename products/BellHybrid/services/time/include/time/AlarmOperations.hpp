// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AlarmOperations.hpp>
#include <db/SystemSettings.hpp>
#include <common/models/BedtimeModel.hpp>

namespace alarms
{
    class AlarmOperationsFactory : public IAlarmOperationsFactory
    {
      public:
        std::unique_ptr<IAlarmOperations> create(
            sys::Service *service,
            std::unique_ptr<AbstractAlarmEventsRepository> &&alarmEventsRepo,
            IAlarmOperations::GetCurrentTime getCurrentTimeCallback) const override;
    };

    class PreWakeUpSettingsProvider
    {
      public:
        struct Settings
        {
            bool enabled{false};
            std::chrono::minutes timeBeforeAlarm{std::chrono::minutes::zero()};
        };

        virtual ~PreWakeUpSettingsProvider() noexcept    = default;
        virtual auto getChimeSettings() -> Settings      = 0;
        virtual auto getFrontlightSettings() -> Settings = 0;
    };

    class SnoozeChimeSettingsProvider
    {
      public:
        struct Settings
        {
            bool enabled{false};
            std::chrono::minutes chimeInterval{std::chrono::minutes::zero()};
        };

        virtual ~SnoozeChimeSettingsProvider() noexcept = default;
        virtual auto getSettings() -> Settings          = 0;
    };

    class AbstractBedtimeSettingsProvider
    {
      public:
        virtual ~AbstractBedtimeSettingsProvider() noexcept = default;
        virtual auto isBedtimeEnabled() -> bool             = 0;
        virtual auto getBedtimeTime() -> time_t             = 0;
    };

    class PreWakeUp
    {
      public:
        struct Decision
        {
            bool timeForChime;
            bool timeForFrontlight;
        };

        explicit PreWakeUp(std::unique_ptr<PreWakeUpSettingsProvider> &&settingsProvider);
        auto decide(TimePoint now, const SingleEventRecord &event) -> Decision;
        auto isActive() const -> bool;

      private:
        auto isTimeForPreWakeUp(TimePoint now,
                                const SingleEventRecord &event,
                                PreWakeUpSettingsProvider::Settings settings) -> bool;

        std::unique_ptr<PreWakeUpSettingsProvider> settingsProvider;
        bool active{false};
    };

    class Bedtime
    {
      public:
        explicit Bedtime(std::unique_ptr<AbstractBedtimeSettingsProvider> &&settingsProvider);
        auto decide(TimePoint now) -> bool;

      private:
        auto isTimeForBed(const TimePoint &now, const time_t &bedtime) -> bool;

        const std::unique_ptr<AbstractBedtimeSettingsProvider> settingsProvider;
    };

    class AlarmOperations : public AlarmOperationsCommon
    {
      public:
        AlarmOperations(std::unique_ptr<AbstractAlarmEventsRepository> &&alarmEventsRepo,
                        GetCurrentTime getCurrentTimeCallback,
                        std::unique_ptr<PreWakeUpSettingsProvider> &&preWakeUpSettingsProvider,
                        std::unique_ptr<SnoozeChimeSettingsProvider> &&snoozeChimeSettingsProvider,
                        std::unique_ptr<AbstractBedtimeSettingsProvider> &&BedtimeModel);

      private:
        void minuteUpdated(TimePoint now) override;
        void stopAllSnoozedAlarms() override;
        bool processPreWakeUp(TimePoint now);
        bool processSnoozeChime(TimePoint now);
        void stopAllSnoozeChimes();

        SingleEventRecord getNextPreWakeUpEvent();
        void handlePreWakeUp(const SingleEventRecord &event, PreWakeUp::Decision decision);
        void handleSnoozeChime(const SingleEventRecord &event, bool newStateOn);
        void handleBedtime(const SingleEventRecord &event, bool decision);
        void processBedtime(TimePoint now);
        void onAlarmTurnedOff(const std::shared_ptr<AlarmEventRecord> &event, alarms::AlarmType alarmType) override;
        void handleAlarmEvent(const std::shared_ptr<AlarmEventRecord> &event,
                              alarms::AlarmType alarmType,
                              bool newStateOn) override;

        bool isBedtimeAllowed() const;

        PreWakeUp preWakeUp;
        std::unique_ptr<SnoozeChimeSettingsProvider> snoozeChimeSettings;
        Bedtime bedtime;
    };
} // namespace alarms
