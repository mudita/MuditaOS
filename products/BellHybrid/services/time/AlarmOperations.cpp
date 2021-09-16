// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <time/AlarmOperations.hpp>

#include <BellAlarmHandler.hpp>

namespace alarms
{
    namespace
    {
        class FakePreWakeUpSettingsProvider : public PreWakeUpSettingsProvider
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
        auto settingsProvider = std::make_unique<FakePreWakeUpSettingsProvider>();
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
          preWakeUp(std::move(settingsProvider))
    {}

    void AlarmOperations::minuteUpdated(TimePoint now)
    {
        AlarmOperationsCommon::minuteUpdated(now);
        processPreWakeUp(now);
    }

    void AlarmOperations::processPreWakeUp(TimePoint now)
    {
        if (nextSingleEvents.empty()) {
            return;
        }

        auto nextEvent = *(nextSingleEvents.front());
        if (getAlarmEventType(nextEvent) != alarms::AlarmType::Clock) {
            return;
        }

        const auto decision = preWakeUp.decide(now, nextEvent);
        if (!decision.timeForChime && !decision.timeForFrontlight) {
            return;
        }
        handlePreWakeUp(nextEvent, decision);
    }

    void AlarmOperations::handlePreWakeUp(const SingleEventRecord &event, PreWakeUp::Decision decision)
    {
        if (auto alarmEventPtr = std::dynamic_pointer_cast<AlarmEventRecord>(event.parent); alarmEventPtr) {
            if (decision.timeForChime) {
                handleAlarmEvent(alarmEventPtr, alarms::AlarmType::PreWakeUpChime, true);
            }
            if (decision.timeForFrontlight) {
                handleAlarmEvent(alarmEventPtr, alarms::AlarmType::PreWakeUpFrontlight, true);
            }
        }
    }

    PreWakeUp::PreWakeUp(std::unique_ptr<PreWakeUpSettingsProvider> &&settingsProvider)
        : settingsProvider{std::move(settingsProvider)}
    {}

    auto PreWakeUp::decide(TimePoint now, const SingleEventRecord &event) -> Decision
    {
        const auto chimeSettings       = settingsProvider->getChimeSettings();
        const auto frontlightSettings  = settingsProvider->getFrontlightSettings();
        const auto isTimeForChime      = isTimeForPreWakeUp(now, event, chimeSettings);
        const auto isTimeForFrontlight = isTimeForPreWakeUp(now, event, frontlightSettings);
        return {isTimeForChime, isTimeForFrontlight};
    }

    auto PreWakeUp::isTimeForPreWakeUp(TimePoint now,
                                       const SingleEventRecord &event,
                                       PreWakeUpSettingsProvider::Settings settings) -> bool
    {
        const auto expectedAlarmStart = std::chrono::floor<std::chrono::minutes>(now) + settings.timeBeforeAlarm;
        return settings.enabled && std::chrono::floor<std::chrono::minutes>(event.startDate) == expectedAlarmStart;
    }
} // namespace alarms
