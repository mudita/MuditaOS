// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ProgressTimer.hpp"
#include <Text.hpp>
#include <ProgressBar.hpp>
#include <ApplicationCommon.hpp>
#include <apps-common/GuiTimer.hpp>
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
                                 ProgressCountdownMode countdownMode,
                                 utils::time::Duration::DisplayedFormat displayFormat)
        : app{app}, parent{parent}, name{std::move(timerName)}, baseTickInterval{baseTick},
          countdownMode{countdownMode}, displayFormat{displayFormat}
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
        timerText += remainingDuration.str(displayFormat);
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

    void ProgressTimer::reset(std::chrono::seconds _duration,
                              std::chrono::seconds _interval,
                              std::chrono::seconds _elapsed)
    {
        Expects(_duration != std::chrono::seconds::zero());

        duration    = _duration;
        elapsed     = _elapsed;
        interval    = _interval;
        hasInterval = _interval != std::chrono::seconds::zero();

        updateText();
        if (_interval != std::chrono::seconds::zero()) {
            updateProgress();
        }
        else {
            resetProgress();
        }
    }

    void ProgressTimer::start()
    {
        if (isStarted) {
            isRunning = true;
            return;
        }
        startTimer();
        isRunning = true;
        isStarted = true;
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
            isStarted = false;
            isRunning = false;
            if (isFinished() && onFinishedCallback != nullptr) {
                onFinishedCallback();
            }
            return true;
        }

        ++elapsed;
        if ((intervalReached() || isFinished()) && onIntervalCallback != nullptr) {
            onIntervalCallback();
        }
        if (onBaseTickCallback != nullptr) {
            onBaseTickCallback();
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

    void ProgressTimer::registerOnBaseTickCallback(std::function<void()> cb)
    {
        onBaseTickCallback = std::move(cb);
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
