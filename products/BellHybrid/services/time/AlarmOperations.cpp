// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <time/AlarmOperations.hpp>

#include <BellAlarmHandler.hpp>

#include <service-db/Settings.hpp>
#include <db/SystemSettings.hpp>
#include <time/dateCommon.hpp>

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
                : bedtimeModel{std::make_unique<app::bell_bedtime::BedtimeModel>(service)}
            {}
            auto isBedtimeEnabled() -> bool override
            {
                return bedtimeModel.get()->getBedtimeOnOff().getValue();
            }
            auto getBedtimeTime() -> time_t override
            {
                return bedtimeModel.get()->getBedtimeTime().getValue();
            }

          private:
            std::unique_ptr<app::bell_bedtime::BedtimeModel> bedtimeModel;
        };
    } // namespace

    namespace
    {
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
    } // namespace

    std::unique_ptr<IAlarmOperations> AlarmOperationsFactory::create(
        sys::Service *service,
        std::unique_ptr<AbstractAlarmEventsRepository> &&alarmEventsRepo,
        IAlarmOperations::GetCurrentTime getCurrentTimeCallback) const
    {
        auto preWakeUpSettingsProvider   = std::make_unique<PreWakeUpSettingsProviderImpl>(service);
        auto snoozeChimeSettingsProvider = std::make_unique<SnoozeChimeSettingsProviderImpl>(service);
        auto bedtimeSettingsProvider     = std::make_unique<BedtimeSettingsProviderImpl>(service);
        auto alarmOperations             = std::make_unique<AlarmOperations>(std::move(alarmEventsRepo),
                                                                 getCurrentTimeCallback,
                                                                 std::move(preWakeUpSettingsProvider),
                                                                 std::move(snoozeChimeSettingsProvider),
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
                                     std::unique_ptr<AbstractBedtimeSettingsProvider> &&bedtimeSettingsProvider)
        : AlarmOperationsCommon{std::move(alarmEventsRepo), std::move(getCurrentTimeCallback)},
          preWakeUp(std::move(preWakeUpSettingsProvider)), snoozeChimeSettings(std::move(snoozeChimeSettings)),
          bedtime(std::move(bedtimeSettingsProvider))
    {}

    void AlarmOperations::minuteUpdated(TimePoint now)
    {
        AlarmOperationsCommon::minuteUpdated(now);
        processPreWakeUp(now);
        processSnoozeChime(now);
        processBedtime(now);
    }

    void AlarmOperations::stopAllSnoozedAlarms()
    {
        stopAllSnoozeChimes();
        AlarmOperationsCommon::stopAllSnoozedAlarms();
    }

    void AlarmOperations::processPreWakeUp(TimePoint now)
    {
        if (nextSingleEvents.empty()) {
            return;
        }

        auto nextEvent = getNextPreWakeUpEvent();
        if (!nextEvent.isValid()) {
            return;
        }

        const auto decision = preWakeUp.decide(now, nextEvent);
        if (!decision.timeForChime && !decision.timeForFrontlight) {
            return;
        }
        handlePreWakeUp(nextEvent, decision);
    }

    void AlarmOperations::processBedtime(TimePoint now)
    {
        if (bedtime.decide(now)) {
            auto bedtimeEvent           = std::make_shared<AlarmEventRecord>();
            bedtimeEvent.get()->enabled = true;
            handleAlarmEvent(bedtimeEvent, alarms::AlarmType::BedtimeReminder, true);
        }
    }

    SingleEventRecord AlarmOperations::getNextPreWakeUpEvent()
    {
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

    void AlarmOperations::processSnoozeChime(TimePoint now)
    {
        if (!ongoingSingleEvents.empty()) {
            return;
        }

        auto settings = snoozeChimeSettings->getSettings();
        if (!settings.enabled) {
            return;
        }

        for (auto &snoozedEvent : snoozedSingleEvents) {
            const auto timeDelta      = TimePointFloorMinutes(now) - TimePointFloorMinutes(snoozedEvent->snoozeStart);
            const auto minuteDelta    = std::chrono::duration_cast<std::chrono::minutes>(timeDelta).count();
            const auto isTimeForChime = (minuteDelta % settings.chimeInterval.count()) == 0;
            if (isTimeForChime) {
                handleSnoozeChime(*snoozedEvent, true);
            }
        }
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

        handleAlarmEvent(event, alarms::AlarmType::PreWakeUpChime, false);
        handleAlarmEvent(event, alarms::AlarmType::PreWakeUpFrontlight, false);
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

    Bedtime::Bedtime(std::unique_ptr<AbstractBedtimeSettingsProvider> &&settingsProvider)
        : settingsProvider{std::move(settingsProvider)}
    {}

    auto Bedtime::decide(TimePoint now) -> bool
    {
        const auto activated = settingsProvider->isBedtimeEnabled();
        const auto time      = settingsProvider->getBedtimeTime();
        return activated && isTimeForBed(now, time);
    }

    auto Bedtime::isTimeForBed(const TimePoint &now, const time_t &bedtime) -> bool
    {
        auto time_tNow    = TimePointToTimeT(now);
        std::tm bedtimeTm = *std::localtime(&bedtime);
        std::tm checkTm   = *std::localtime(&time_tNow);
        return (bedtimeTm.tm_hour == checkTm.tm_hour && bedtimeTm.tm_min == checkTm.tm_min);
    }
} // namespace alarms
