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

    class PreWakeUpHandler
    {
      public:
        struct Result
        {
            bool timeForChime;
            bool timeForFrontlight;
        };

        explicit PreWakeUpHandler(PreWakeUpSettingsProvider *settingsProvider);
        auto handle(TimePoint now, const SingleEventRecord &event) -> Result;

      private:
        auto isTimeForPreWakeUp(TimePoint now,
                                const SingleEventRecord &event,
                                PreWakeUpSettingsProvider::Settings settings) -> bool;

        PreWakeUpSettingsProvider *settingsProvider;
    };

    class AlarmOperations : public AlarmOperationsCommon
    {
      public:
        AlarmOperations(std::unique_ptr<AbstractAlarmEventsRepository> &&alarmEventsRepo,
                        GetCurrentTime getCurrentTimeCallback,
                        std::unique_ptr<PreWakeUpSettingsProvider> &&settingsProvider);

        void minuteUpdated(TimePoint now) override;

      private:
        void processPreWakeUpEvents(TimePoint now);
        void preWakeUp(const SingleEventRecord &event, PreWakeUpHandler::Result handleResult);

        std::unique_ptr<PreWakeUpSettingsProvider> settingsProvider;
    };
} // namespace alarms
