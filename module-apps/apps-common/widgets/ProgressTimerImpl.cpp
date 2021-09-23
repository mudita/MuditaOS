// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ProgressTimerImpl.hpp"
#include <Text.hpp>
#include <ProgressBar.hpp>
#include <ApplicationCommon.hpp>
#include <time/time_conversion.hpp>
#include <gsl/assert>

namespace app
{

    ProgressTimerImpl::ProgressTimerImpl(app::ApplicationCommon *app,
                                         gui::Item *parent,
                                         std::string timerName,
                                         std::chrono::milliseconds baseTick)
        : app{app}, parent{parent}, name{std::move(timerName)}, baseTickInterval{baseTick}
    {}

    void ProgressTimerImpl::resetProgress()
    {
        if (progress != nullptr) {
            progress->setValue(0);
        }
    }

    void ProgressTimerImpl::update()
    {
        updateText();
        updateProgress();
        app->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
    }

    void ProgressTimerImpl::updateText()
    {
        using utils::time::Duration;
        if (text != nullptr) {
            const auto secondsRemaining = duration - elapsed;
            const Duration remainingDuration{std::time_t{secondsRemaining.count()}};
            text->setText(remainingDuration.str(Duration::DisplayedFormat::Fixed0M0S));
        }
    }

    void ProgressTimerImpl::updateProgress()
    {
        if (progress != nullptr) {
            const auto percentage  = static_cast<float>(elapsed.count()) / duration.count();
            const auto currentStep = percentage * progress->getMaximum();
            progress->setValue(std::ceil(currentStep));
        }
    }

    void ProgressTimerImpl::reset(std::chrono::seconds _duration,
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

    void ProgressTimerImpl::start()
    {
        if (isStarted) {
            isRunning = true;
            return;
        }
        startTimer();
        isRunning = true;
        isStarted = true;
    }

    void ProgressTimerImpl::startTimer()
    {
        Expects(app != nullptr);
        parent->timerCallback = [this](gui::Item &it, sys::Timer &task) { return onTimerTimeout(it, task); };
        timerTask             = app::GuiTimerFactory::createPeriodicTimer(app, parent, name, baseTickInterval);
        timerTask.start();
    }

    auto ProgressTimerImpl::onTimerTimeout(gui::Item &self, sys::Timer &task) -> bool
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

    auto ProgressTimerImpl::isFinished() const noexcept -> bool
    {
        return duration <= elapsed;
    }

    auto ProgressTimerImpl::isStopped() const noexcept -> bool
    {
        return !isRunning;
    }

    auto ProgressTimerImpl::intervalReached() const noexcept -> bool
    {
        return hasInterval && (elapsed.count() % interval.count()) == 0;
    }

    void ProgressTimerImpl::stop()
    {
        isRunning = false;
    }

    void ProgressTimerImpl::registerOnFinishedCallback(std::function<void()> cb)
    {
        onFinishedCallback = std::move(cb);
    }

    void ProgressTimerImpl::registerOnIntervalCallback(std::function<void()> cb)
    {
        onIntervalCallback = std::move(cb);
    }

    void ProgressTimerImpl::registerOnBaseTickCallback(std::function<void()> cb)
    {
        onBaseTickCallback = std::move(cb);
    }

    void ProgressTimerImpl::attach(gui::Progress *_progress)
    {
        Expects(_progress != nullptr);
        progress = _progress;
    }

    void ProgressTimerImpl::attach(gui::Text *_text)
    {
        Expects(_text != nullptr);
        text = _text;
    }
} // namespace app
