// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AlarmOperations.hpp>

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

      private:
        auto isTimeForPreWakeUp(TimePoint now,
                                const SingleEventRecord &event,
                                PreWakeUpSettingsProvider::Settings settings) -> bool;

        std::unique_ptr<PreWakeUpSettingsProvider> settingsProvider;
    };

    class AlarmOperations : public AlarmOperationsCommon
    {
      public:
        AlarmOperations(std::unique_ptr<AbstractAlarmEventsRepository> &&alarmEventsRepo,
                        GetCurrentTime getCurrentTimeCallback,
                        std::unique_ptr<PreWakeUpSettingsProvider> &&settingsProvider);

        void minuteUpdated(TimePoint now) override;
        void processPreWakeUp(TimePoint now);

      private:
        void handlePreWakeUp(const SingleEventRecord &event, PreWakeUp::Decision decision);

        PreWakeUp preWakeUp;
    };
} // namespace alarms
