// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "PlayAudioActions.hpp"

#include <Paths.hpp>
#include <audio/AudioMessage.hpp>
#include <db/SystemSettings.hpp>
#include <Timers/TimerFactory.hpp>

namespace
{
    constexpr std::chrono::seconds alarmFadeDuration{45};
} // namespace

namespace alarms
{
    PlayAudioAction::PlayAudioAction(sys::Service &service,
                                     std::string_view toneSetting,
                                     const audio::PlaybackType &playbackType,
                                     const audio::PlaybackMode &playbackMode,
                                     std::optional<std::string_view> durationSetting)
        : service{service}, toneSetting{toneSetting}, durationSetting{durationSetting}, playbackType{playbackType},
          playbackMode{playbackMode}, settings{service::ServiceProxy{service.weak_from_this()}}
    {}

    auto PlayAudioAction::play(const std::filesystem::path &path, std::optional<std::chrono::minutes> duration) -> bool
    {
        if (duration.has_value()) {
            spawnTimer(duration.value());
        }
        const auto fadeEnabledInSettings = utils::getNumericValue<bool>(
            settings.getValue(bell::settings::Alarm::fadeActive, settings::SettingsScope::Global));
        const auto fadeInEnabled = (fadeEnabledInSettings && (playbackType == audio::PlaybackType::Alarm))
                                       ? audio::Fade::In
                                       : audio::Fade::Disable;

        auto msg = std::make_shared<service::AudioStartPlaybackRequest>(
            path, playbackType, playbackMode, audio::FadeParams{fadeInEnabled, alarmFadeDuration});
        return service.bus.sendUnicast(std::move(msg), service::audioServiceName);
    }

    auto PlayAudioAction::turnOff() -> bool
    {
        detachTimer();
        auto stopPlaybackVec = std::vector<audio::PlaybackType>({playbackType});

        auto msg = std::make_shared<service::AudioStopRequest>(std::move(stopPlaybackVec));
        return service.bus.sendUnicast(std::move(msg), service::audioServiceName);
    }

    auto PlayAudioAction::execute() -> bool
    {
        auto path = settings.getValue(toneSetting, settings::SettingsScope::Global);
        if (!std::filesystem::exists(path)) {
            const auto &fallbackPath = handleMissingFile();
            LOG_ERROR("File '%s' not found, falling back to default: '%s'", path.c_str(), fallbackPath.c_str());
            path = fallbackPath;
        }

        std::optional<std::chrono::minutes> duration{};
        if (durationSetting.has_value()) {
            const auto &durationStr = settings.getValue(durationSetting.value(), settings::SettingsScope::Global);
            duration.emplace(utils::getNumericValue<int>(durationStr));
        }

        return play(path, duration);
    }

    auto PlayAudioAction::detachTimer() -> void
    {
        if (timer.isValid()) {
            timer.stop();
            timer.reset();
        }
    }

    auto PlayAudioAction::spawnTimer(std::chrono::minutes timeout) -> void
    {
        constexpr auto timerName = "playDurationTimer";
        if (!timer.isValid()) {
            auto callback = [this](sys::Timer &) { turnOff(); };
            timer         = sys::TimerFactory::createSingleShotTimer(&service, timerName, timeout, callback);
        }
        timer.stop();
        timer.start();
    }

    auto PlayAudioAction::getFallbackTonePath() -> std::string
    {
        using namespace settings;
        using namespace bell::settings;

        if (toneSetting == Snooze::tonePath) {
            return settings.getValue(Snooze::toneFallbackPath, SettingsScope::Global);
        }
        if (toneSetting == PrewakeUp::tonePath) {
            return settings.getValue(PrewakeUp::toneFallbackPath, SettingsScope::Global);
        }
        if (toneSetting == Alarm::tonePath) {
            return settings.getValue(Alarm::toneFallbackPath, SettingsScope::Global);
        }
        if (toneSetting == Bedtime::tonePath) {
            return settings.getValue(Bedtime::toneFallbackPath, SettingsScope::Global);
        }
        return {};
    }

    auto PlayAudioAction::handleMissingFile() -> std::string
    {
        auto fallbackPath = getFallbackTonePath();
        if (fallbackPath.empty()) {
            LOG_ERROR("Fallback path not found! This should never really happen!");
        }
        else {
            settings.setValue(toneSetting, fallbackPath, settings::SettingsScope::Global);
        }
        return fallbackPath;
    }

    namespace factory
    {
        std::unique_ptr<PlayAudioAction> createPreWakeUpChimeAction(sys::Service &service)
        {
            return std::make_unique<PlayAudioAction>(
                service, bell::settings::PrewakeUp::tonePath, audio::PlaybackType::PreWakeUp);
        }

        std::unique_ptr<PlayAudioAction> createSnoozeChimeAction(sys::Service &service)
        {
            return std::make_unique<PlayAudioAction>(
                service, bell::settings::Snooze::tonePath, audio::PlaybackType::Snooze);
        }

        std::unique_ptr<PlayAudioAction> createAlarmToneAction(sys::Service &service)
        {
            /// Alarm duration is controlled from the main application's state machine
            return std::make_unique<PlayAudioAction>(
                service, bell::settings::Alarm::tonePath, audio::PlaybackType::Alarm);
        }

        std::unique_ptr<PlayAudioAction> createBedtimeChimeAction(sys::Service &service)
        {
            return std::make_unique<PlayAudioAction>(
                service, bell::settings::Bedtime::tonePath, audio::PlaybackType::Bedtime);
        }
    } // namespace factory
} // namespace alarms
