// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <time/AlarmOperations.hpp>

#include <BellAlarmHandler.hpp>

namespace alarms
{
    namespace
    {
        class MockedPreWakeUpSettingsProvider : public PreWakeUpSettingsProvider
        {
          public:
            auto getChimeSettings() -> Settings override
            {
                return {true, std::chrono::minutes{5}};
            }

            auto getFrontlightSettings() -> Settings override
            {
                return {true, std::chrono::minutes{5}};
            }
        };
    } // namespace

    std::unique_ptr<IAlarmOperations> AlarmOperationsFactory::create(
        sys::Service *service,
        std::unique_ptr<AbstractAlarmEventsRepository> &&alarmEventsRepo,
        IAlarmOperations::GetCurrentTime getCurrentTimeCallback) const
    {
        auto settingsProvider = std::make_unique<MockedPreWakeUpSettingsProvider>();
        auto alarmOperations  = std::make_unique<AlarmOperations>(
            std::move(alarmEventsRepo), getCurrentTimeCallback, std::move(settingsProvider));
        alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::PreWakeUpChime,
                                                  std::make_shared<alarms::PreWakeUpChimeHandler>(service));
        alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::PreWakeUpFrontlight,
                                                  std::make_shared<alarms::PreWakeUpFrontlightHandler>(service));
        return alarmOperations;
    }

    AlarmOperations::AlarmOperations(std::unique_ptr<AbstractAlarmEventsRepository> &&alarmEventsRepo,
                                     GetCurrentTime getCurrentTimeCallback,
                                     std::unique_ptr<PreWakeUpSettingsProvider> &&settingsProvider)
        : AlarmOperationsCommon{std::move(alarmEventsRepo), std::move(getCurrentTimeCallback)},
          settingsProvider{std::move(settingsProvider)}
    {}

    void AlarmOperations::minuteUpdated(TimePoint now)
    {
        AlarmOperationsCommon::minuteUpdated(now);
        processPreWakeUpEvents(now);
    }

    void AlarmOperations::processPreWakeUpEvents(TimePoint now)
    {
        if (nextSingleEvents.empty()) {
            return;
        }

        auto nextEvent = *(nextSingleEvents.front());
        if (getAlarmEventType(nextEvent) != alarms::AlarmType::Clock) {
            return;
        }

        PreWakeUpHandler handler{settingsProvider.get()};
        preWakeUp(nextEvent, handler.handle(now, nextEvent));
    }

    void AlarmOperations::preWakeUp(const SingleEventRecord &event, PreWakeUpHandler::Result handleResult)
    {
        if (!handleResult.timeForChime && !handleResult.timeForFrontlight) {
            return;
        }

        if (auto alarmEventPtr = std::dynamic_pointer_cast<AlarmEventRecord>(event.parent); alarmEventPtr) {
            if (handleResult.timeForChime) {
                handleAlarmEvent(alarmEventPtr, alarms::AlarmType::PreWakeUpChime, true);
            }
            if (handleResult.timeForFrontlight) {
                handleAlarmEvent(alarmEventPtr, alarms::AlarmType::PreWakeUpFrontlight, true);
            }
        }
    }

    PreWakeUpHandler::PreWakeUpHandler(PreWakeUpSettingsProvider *settingsProvider) : settingsProvider{settingsProvider}
    {}

    auto PreWakeUpHandler::handle(TimePoint now, const SingleEventRecord &event) -> Result
    {
        const auto chimeSettings       = settingsProvider->getChimeSettings();
        const auto frontlightSettings  = settingsProvider->getFrontlightSettings();
        const auto isTimeForChime      = isTimeForPreWakeUp(now, event, chimeSettings);
        const auto isTimeForFrontlight = isTimeForPreWakeUp(now, event, frontlightSettings);
        return {isTimeForChime, isTimeForFrontlight};
    }

    auto PreWakeUpHandler::isTimeForPreWakeUp(TimePoint now,
                                              const SingleEventRecord &event,
                                              PreWakeUpSettingsProvider::Settings settings) -> bool
    {
        const auto expectedAlarmStart = std::chrono::floor<std::chrono::minutes>(now) + settings.timeBeforeAlarm;
        return settings.enabled && std::chrono::floor<std::chrono::minutes>(event.startDate) == expectedAlarmStart;
    }
} // namespace alarms
