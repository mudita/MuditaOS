// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <BellAlarmHandler.hpp>

#include <time/AlarmOperations.hpp>
#include <time/dateCommon.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>

#include <string>

namespace alarms
{
    namespace
    {
        class PreWakeUpSettingsProviderImpl : public PreWakeUpSettingsProvider
        {
          public:
            explicit PreWakeUpSettingsProviderImpl(sys::Service *service);
            auto getChimeSettings() -> Settings override;
            auto getFrontlightSettings() -> Settings override;

          private:
            auto getSettings(std::string_view path) -> Settings;

            settings::Settings settings;
        };

        PreWakeUpSettingsProviderImpl::PreWakeUpSettingsProviderImpl(sys::Service *service)
        {
            settings.init(service::ServiceProxy{service->weak_from_this()});
        }

        auto PreWakeUpSettingsProviderImpl::getChimeSettings() -> Settings
        {
            return getSettings(bell::settings::PrewakeUp::duration);
        }

        auto PreWakeUpSettingsProviderImpl::getFrontlightSettings() -> Settings
        {
            return getSettings(bell::settings::PrewakeUp::lightDuration);
        }

        auto PreWakeUpSettingsProviderImpl::getSettings(std::string_view path) -> Settings
        {
            const auto settingValue = settings.getValue(path.data(), settings::SettingsScope::Global);
            const auto duration     = std::chrono::minutes{utils::toNumeric(settingValue)};
            const auto isEnabled    = duration != std::chrono::minutes::zero();
            LOG_DEBUG(
                "%s: enabled: %d; duration: %d minutes", path.data(), isEnabled, static_cast<int>(duration.count()));
            return {isEnabled, duration};
        }

        class BedtimeSettingsProviderImpl : public AbstractBedtimeSettingsProvider
        {
          public:
            explicit BedtimeSettingsProviderImpl(sys::Service *service)
                : bedtimeTimeModel{std::make_unique<app::bell_bedtime::BedtimeTimeModel>(service)}
            {
                settings.init(service::ServiceProxy{service->weak_from_this()});
            }

            auto isBedtimeEnabled() -> bool override
            {
                bool enabled = false;
                try {
                    enabled =
                        std::stoi(settings.getValue(bell::settings::Bedtime::active, settings::SettingsScope::Global));
                }
                catch (const std::exception &e) {
                    LOG_ERROR("Active db record not valid! err: %s", e.what());
                }
                return enabled;
            }

            auto getBedtimeTime() -> std::time_t override
            {
                return bedtimeTimeModel->getValue();
            }

          private:
            std::unique_ptr<app::bell_bedtime::BedtimeTimeModel> bedtimeTimeModel;
            settings::Settings settings;
        };

        class SnoozeChimeSettingsProviderImpl : public SnoozeChimeSettingsProvider
        {
          public:
            explicit SnoozeChimeSettingsProviderImpl(sys::Service *service);
            auto getSettings() -> Settings override;

          private:
            settings::Settings settings;
        };

        SnoozeChimeSettingsProviderImpl::SnoozeChimeSettingsProviderImpl(sys::Service *service)
        {
            settings.init(service::ServiceProxy{service->weak_from_this()});
        }

        auto SnoozeChimeSettingsProviderImpl::getSettings() -> Settings
        {
            const auto intervalStr =
                settings.getValue(bell::settings::Snooze::interval, settings::SettingsScope::Global);
            const auto interval = utils::getNumericValue<std::uint32_t>(intervalStr);
            if (interval == 0) {
                return {false, std::chrono::minutes{0}};
            }
            else {
                return {true, std::chrono::minutes{interval}};
            }
        }

        class OnboardingSettingsProviderImpl : public OnboardingSettingsProvider
        {
          public:
            explicit OnboardingSettingsProviderImpl(sys::Service *service);
            auto isDone() -> bool override;

          private:
            settings::Settings settings;
        };

        OnboardingSettingsProviderImpl::OnboardingSettingsProviderImpl(sys::Service *service)
        {
            settings.init(service::ServiceProxy{service->weak_from_this()});
        }

        auto OnboardingSettingsProviderImpl::isDone() -> bool
        {
            return utils::getNumericValue<bool>(
                settings.getValue(settings::SystemProperties::onboardingDone, settings::SettingsScope::Global));
        }

    } // namespace

    std::unique_ptr<IAlarmOperations> AlarmOperationsFactory::create(
        sys::Service *service,
        std::unique_ptr<AbstractAlarmEventsRepository> &&alarmEventsRepo,
        IAlarmOperations::GetCurrentTime getCurrentTimeCallback) const
    {
        auto preWakeUpSettingsProvider   = std::make_unique<PreWakeUpSettingsProviderImpl>(service);
        auto snoozeChimeSettingsProvider = std::make_unique<SnoozeChimeSettingsProviderImpl>(service);
        auto onboardingSettingsProvider  = std::make_unique<OnboardingSettingsProviderImpl>(service);
        auto bedtimeSettingsProvider     = std::make_unique<BedtimeSettingsProviderImpl>(service);
        auto alarmOperations             = std::make_unique<AlarmOperations>(std::move(alarmEventsRepo),
                                                                 getCurrentTimeCallback,
                                                                 std::move(preWakeUpSettingsProvider),
                                                                 std::move(snoozeChimeSettingsProvider),
                                                                 std::move(onboardingSettingsProvider),
                                                                 std::move(bedtimeSettingsProvider));
        alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::Clock,
                                                  std::make_shared<alarms::BellAlarmClockHandler>(service));
        alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::PreWakeUpChime,
                                                  std::make_shared<alarms::PreWakeUpChimeHandler>(service));
        alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::PreWakeUpFrontlight,
                                                  std::make_shared<alarms::PreWakeUpFrontlightHandler>(service));
        alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::SnoozeChime,
                                                  std::make_shared<alarms::SnoozeChimeHandler>(service));
        alarmOperations->addAlarmExecutionHandler(alarms::AlarmType::BedtimeReminder,
                                                  std::make_shared<alarms::BedtimeReminderHandler>(service));

        return alarmOperations;
    }

    AlarmOperations::AlarmOperations(std::unique_ptr<AbstractAlarmEventsRepository> &&alarmEventsRepo,
                                     GetCurrentTime getCurrentTimeCallback,
                                     std::unique_ptr<PreWakeUpSettingsProvider> &&preWakeUpSettingsProvider,
                                     std::unique_ptr<SnoozeChimeSettingsProvider> &&snoozeChimeSettings,
                                     std::unique_ptr<OnboardingSettingsProvider> &&onboardingSettings,
                                     std::unique_ptr<AbstractBedtimeSettingsProvider> &&bedtimeSettingsProvider)
        : AlarmOperationsCommon{std::move(alarmEventsRepo), std::move(getCurrentTimeCallback)},
          preWakeUp(std::move(preWakeUpSettingsProvider)), snoozeChimeSettings(std::move(snoozeChimeSettings)),
          onboardingSettings(std::move(onboardingSettings)), bedtime(std::move(bedtimeSettingsProvider))
    {}

    void AlarmOperations::minuteUpdated(TimePoint now)
    {
        // Prevent activating alarms when the onboard is not done yet
        if (!isOnboardingDone()) {
            return;
        }
        /**
         * A very simple alarm priority:
         * 1. Main alarm
         * 2. Pre wake up
         * 3. Snooze
         * 4. Bedtime
         * By design, there is no possibility to have both the main alarm, pre wakeup, and snooze alarm set for the same
         * timestamp hence it is safe to process these three in the one go.
         */
        AlarmOperationsCommon::minuteUpdated(now);
        auto prewakeupStarted  = processPreWakeUp(now);
        auto snoozeChimePlayed = processSnoozeChime(now);

        // If we are during snooze or its prewakeup time, we decided to skip bedtime handling at all
        if (!prewakeupStarted && !snoozeChimePlayed && isBedtimeAllowed()) {
            processBedtime(now);
        }
    }

    void AlarmOperations::stopAllSnoozedAlarms()
    {
        stopAllSnoozeChimes();
        AlarmOperationsCommon::stopAllSnoozedAlarms();
    }

    bool AlarmOperations::processPreWakeUp(TimePoint now)
    {
        auto nextEvent = getNextPreWakeUpEvent();
        if (!nextEvent.isValid()) {
            return false;
        }

        const auto decision = preWakeUp.decide(now, nextEvent);
        if (!decision.timeForChime && !decision.timeForFrontlight) {
            return false;
        }
        preWakeUp.setActive(true);
        handlePreWakeUp(nextEvent, decision);
        return true;
    }

    void AlarmOperations::processBedtime(TimePoint now)
    {
        if (bedtime.decide(now)) {
            auto bedtimeEvent           = std::make_shared<AlarmEventRecord>();
            bedtimeEvent->enabled       = true;
            handleAlarmEvent(bedtimeEvent, alarms::AlarmType::BedtimeReminder, true);
        }
    }

    SingleEventRecord AlarmOperations::getNextPreWakeUpEvent()
    {
        if (nextSingleEvents.empty()) {
            return {};
        }

        const auto event = *(nextSingleEvents.front());
        if (getAlarmEventType(event) != alarms::AlarmType::Clock) {
            return {};
        }

        auto found = std::find_if(snoozedSingleEvents.begin(), snoozedSingleEvents.end(), [ev = event](auto &event) {
            return ev.parent->ID == event->parent->ID;
        });
        if (found != snoozedSingleEvents.end()) {
            return {};
        }

        return event;
    }

    void AlarmOperations::turnOffRingingAlarm(std::uint32_t id, OnTurnOffRingingAlarm callback)
    {
        auto nextEvent = getNextPreWakeUpEvent();
        if (nextEvent.isValid()) {
            if (auto event = std::dynamic_pointer_cast<AlarmEventRecord>(nextEvent.parent); event) {
                disablePreWakeUp(event);
            }
        }
        AlarmOperationsCommon::turnOffRingingAlarm(id, callback);
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

    auto AlarmOperations::disablePreWakeUp(const std::shared_ptr<AlarmEventRecord> &event) -> void
    {
        if (preWakeUp.isActive()) {
            AlarmOperationsCommon::handleAlarmEvent(event, alarms::AlarmType::PreWakeUpChime, false);
            AlarmOperationsCommon::handleAlarmEvent(event, alarms::AlarmType::PreWakeUpFrontlight, false);
            preWakeUp.setActive(false);
        }
    }

    bool AlarmOperations::processSnoozeChime(TimePoint now)
    {
        if (!ongoingSingleEvents.empty()) {
            return false;
        }

        auto settings = snoozeChimeSettings->getSettings();
        if (!settings.enabled) {
            return false;
        }

        for (auto &snoozedEvent : snoozedSingleEvents) {
            const auto timeDelta      = TimePointFloorMinutes(now) - TimePointFloorMinutes(snoozedEvent->snoozeStart);
            const auto minuteDelta    = std::chrono::duration_cast<std::chrono::minutes>(timeDelta).count();
            const auto isTimeForChime = (minuteDelta % settings.chimeInterval.count()) == 0;
            if (isTimeForChime) {
                handleSnoozeChime(*snoozedEvent, true);
                return true;
            }
        }
        return false;
    }

    void AlarmOperations::handleSnoozeChime(const SingleEventRecord &event, bool newStateOn)
    {
        if (auto alarmEventPtr = std::dynamic_pointer_cast<AlarmEventRecord>(event.parent); alarmEventPtr) {
            handleAlarmEvent(alarmEventPtr, alarms::AlarmType::SnoozeChime, newStateOn);
        }
    }

    void AlarmOperations::stopAllSnoozeChimes()
    {
        for (auto &snoozedEvent : snoozedSingleEvents) {
            handleSnoozeChime(*snoozedEvent, false);
        }
    }

    void AlarmOperations::onAlarmTurnedOff([[maybe_unused]] const std::shared_ptr<AlarmEventRecord> &event,
                                           alarms::AlarmType alarmType)
    {
        if (alarmType != alarms::AlarmType::Clock) {
            return;
        }
        disablePreWakeUp(event);
    }
    bool AlarmOperations::isBedtimeAllowed() const
    {
        return ongoingSingleEvents.empty() && snoozedSingleEvents.empty() && not preWakeUp.isActive();
    }
    bool AlarmOperations::isOnboardingDone()
    {
        return onboardingSettings->isDone();
    }

    void AlarmOperations::handleAlarmEvent(const std::shared_ptr<AlarmEventRecord> &event,
                                           alarms::AlarmType alarmType,
                                           bool newStateOn)
    {
        if (newStateOn && alarmType == alarms::AlarmType::Clock) {
            disablePreWakeUp(event);
        }
        AlarmOperationsCommon::handleAlarmEvent(event, alarmType, newStateOn);
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

    auto PreWakeUp::isActive() const -> bool
    {
        return active;
    }

    auto PreWakeUp::setActive(bool state) -> void
    {
        active = state;
    }

    Bedtime::Bedtime(std::unique_ptr<AbstractBedtimeSettingsProvider> &&settingsProvider)
        : settingsProvider{std::move(settingsProvider)}
    {}

    auto Bedtime::decide(TimePoint now) -> bool
    {
        const auto activated = settingsProvider->isBedtimeEnabled();
        const auto time      = settingsProvider->getBedtimeTime();
        return activated && isTimeForBed(now, time);
    }

    auto Bedtime::isTimeForBed(const TimePoint &now, const std::time_t &bedtime) -> bool
    {
        const auto timeNow   = TimePointToTimeT(now);
        const auto bedtimeTm = *std::localtime(&bedtime);
        const auto checkTm   = *std::localtime(&timeNow);
        return (bedtimeTm.tm_hour == checkTm.tm_hour && bedtimeTm.tm_min == checkTm.tm_min);
    }
} // namespace alarms
