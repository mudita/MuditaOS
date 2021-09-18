// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ProgressTimer.hpp"
#include <Text.hpp>
#include <ProgressBar.hpp>
#include <ApplicationCommon.hpp>
#include <apps-common/GuiTimer.hpp>
#include <time/time_conversion.hpp>
#include <gsl/assert>

namespace
{
    inline constexpr auto increasingModePrefix = "-";
}
namespace app
{

    ProgressTimer::ProgressTimer(app::ApplicationCommon *app,
                                 gui::Item &parent,
                                 std::string timerName,
                                 std::chrono::milliseconds baseTick,
                                 ProgressCountdownMode countdownMode)
        : app{app}, parent{parent}, name{std::move(timerName)}, baseTickInterval{baseTick}, countdownMode{countdownMode}
    {}

    void ProgressTimer::resetProgress()
    {
        if (progress != nullptr) {
            progress->setValue(0);
        }
    }

    void ProgressTimer::update()
    {
        updateText();
        updateProgress();
        app->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
    }

    void ProgressTimer::updateText()
    {
        using utils::time::Duration;
        if (text == nullptr) {
            return;
        }
        const auto secondsRemaining = duration - elapsed;
        const Duration remainingDuration{std::time_t{secondsRemaining.count()}};
        UTF8 timerText;
        if (countdownMode == ProgressCountdownMode::Increasing && secondsRemaining != std::chrono::seconds::zero()) {
            timerText += increasingModePrefix;
        }
        timerText += remainingDuration.str(Duration::DisplayedFormat::Fixed0M0S);
        text->setText(std::move(timerText));
    }

    void ProgressTimer::updateProgress()
    {
        if (progress != nullptr) {
            const auto percentage  = static_cast<float>(elapsed.count()) / duration.count();
            const auto currentStep = percentage * progress->getMaximum();
            progress->setValue(std::ceil(currentStep));
        }
    }

    void ProgressTimer::reset(std::chrono::seconds _duration, std::chrono::seconds _interval)
    {
        Expects(_duration != std::chrono::seconds::zero());

        duration    = _duration;
        elapsed     = std::chrono::seconds::zero();
        interval    = _interval;
        hasInterval = _interval != std::chrono::seconds::zero();

        updateText();
        resetProgress();
    }

    void ProgressTimer::start()
    {
        startTimer();
        isRunning = true;
    }

    void ProgressTimer::startTimer()
    {
        Expects(app != nullptr);
        parent.timerCallback = [this](gui::Item &it, sys::Timer &task) { return onTimerTimeout(task); };
        timerTask            = app::GuiTimerFactory::createPeriodicTimer(app, &parent, name, baseTickInterval);
        timerTask.start();
    }

    auto ProgressTimer::onTimerTimeout(sys::Timer &task) -> bool
    {
        if (isStopped() || isFinished()) {
            task.stop();
            if (isFinished() && onFinishedCallback != nullptr) {
                onFinishedCallback();
            }
            return true;
        }

        ++elapsed;
        if ((intervalReached() || isFinished()) && onIntervalCallback != nullptr) {
            onIntervalCallback();
        }
        update();
        return true;
    }

    auto ProgressTimer::isFinished() const noexcept -> bool
    {
        return duration <= elapsed;
    }

    auto ProgressTimer::isStopped() const noexcept -> bool
    {
        return !isRunning;
    }

    auto ProgressTimer::intervalReached() const noexcept -> bool
    {
        return hasInterval && (elapsed.count() % interval.count()) == 0;
    }

    void ProgressTimer::stop()
    {
        isRunning = false;
    }

    void ProgressTimer::registerOnFinishedCallback(std::function<void()> cb)
    {
        onFinishedCallback = std::move(cb);
    }

    void ProgressTimer::registerOnIntervalCallback(std::function<void()> cb)
    {
        onIntervalCallback = std::move(cb);
    }

    void ProgressTimer::attach(gui::Progress *_progress)
    {
        Expects(_progress != nullptr);
        progress = _progress;
    }

    void ProgressTimer::attach(gui::Text *_text)
    {
        Expects(_text != nullptr);
        text = _text;
    }
} // namespace app
