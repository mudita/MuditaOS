// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ProgressTimer.hpp"
#include <Text.hpp>
#include <ProgressBar.hpp>
#include <ApplicationCommon.hpp>
#include <apps-common/GuiTimer.hpp>
#include <gsl/assert>

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

    void ProgressTimer::update()
    {
        app->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
    }

    void ProgressTimer::reset(std::chrono::seconds _duration, std::chrono::seconds _interval)
    {
        Expects(_duration != std::chrono::seconds::zero());

        duration    = _duration;
        elapsed     = std::chrono::seconds::zero();
        interval    = _interval;
        hasInterval = _interval != std::chrono::seconds::zero();

        update();
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
        ++elapsed;
        update();
        if (isStopped() || isFinished()) {
            task.stop();
            if (isFinished() && onFinishedCallback != nullptr) {
                onFinishedCallback();
            }
            return true;
        }

        if ((intervalReached() || isFinished()) && onIntervalCallback != nullptr) {
            onIntervalCallback();
        }
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
} // namespace app
