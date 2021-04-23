﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationTimer.hpp"
#include "GuiTimer.hpp"

#include <service-audio/AudioServiceAPI.hpp>
#include <application-meditation/data/Style.hpp>
#include <purefs/filesystem_paths.hpp>
#include <time/time_conversion.hpp>

namespace gui
{
    namespace
    {
        constexpr auto TimerInterval = std::chrono::milliseconds{1000};
    } // namespace

    MeditationTimer::MeditationTimer(std::uint32_t x,
                                     std::uint32_t y,
                                     std::uint32_t width,
                                     std::uint32_t height,
                                     app::ApplicationMeditation *app,
                                     Item *_parent)
        : HBox(_parent, x, y, width, height), application{app}
    {
        setEdges(RectangleEdge::None);
        build();
    }

    void MeditationTimer::build()
    {
        namespace timerStyle = style::meditation::timer;

        const Point boxCenter(getX() + (getWidth() / 2), getY() + (getHeight() / 2));
        Circle::ShapeParams params;
        params.setCenterPoint(boxCenter)
            .setRadius(timerStyle::Radius)
            .setBorderColor(timerStyle::BorderColor)
            .setFocusBorderColor(timerStyle::BorderColor)
            .setPenWidth(timerStyle::PenWidth)
            .setFocusPenWidth(timerStyle::PenWidth);
        progressBar = new CircularProgressBar(this, params);

        timer = new Text(progressBar, 0, 0, getWidth(), getHeight());
        timer->setEdges(RectangleEdge::None);
        timer->setFont(style::window::font::supersizemelight);
        timer->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        timer->setEditMode(EditMode::Browse);

        onReset();
    }

    void MeditationTimer::onReset()
    {
        updateTimer();
        progressBar->setMaximum(duration.count());
        progressBar->setValue(0);
    }

    void MeditationTimer::update()
    {
        updateTimer();
        progressBar->setPercentageValue(calculatePercentageValue());
        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
    }

    void MeditationTimer::updateTimer()
    {
        using utils::time::Duration;

        const auto secondsRemaining = duration - elapsed;
        const Duration remainingDuration{std::time_t{secondsRemaining.count()}};
        timer->setText(remainingDuration.str(Duration::DisplayedFormat::Fixed0M0S));
    }

    auto MeditationTimer::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool
    {
        setPosition(newDim.x, newDim.y);
        setSize(newDim.w, newDim.h);
        return true;
    }

    void MeditationTimer::reset(std::chrono::seconds _duration, std::chrono::seconds _intervalPeriod) noexcept
    {
        assert(_duration != std::chrono::seconds::zero()); // Pre-condition check.

        duration       = _duration;
        elapsed        = std::chrono::seconds::zero();
        intervalPeriod = _intervalPeriod;
        hasInterval    = _intervalPeriod != std::chrono::seconds::zero();
        onReset();
    }

    void MeditationTimer::start()
    {
        startTimer();
        isRunning = true;
    }

    void MeditationTimer::startTimer()
    {
        assert(application != nullptr);
        timerCallback = [this](Item &it, sys::Timer &task) { return onTimerTimeout(it, task); };
        timerTask     = app::GuiTimerFactory::createPeriodicTimer(application, this, "MeditationTimer", TimerInterval);
        timerTask.start();
    }

    auto MeditationTimer::onTimerTimeout(Item &self, sys::Timer &task) -> bool
    {
        if (isStopped() || isFinished()) {
            task.stop();
            if (isFinished() && timeoutCallback != nullptr) {
                timeoutCallback();
            }
            return true;
        }

        ++elapsed;
        if (intervalReached() || isFinished()) {
            onInterval();
        }
        update();
        return true;
    }

    auto MeditationTimer::isFinished() const noexcept -> bool
    {
        return duration <= elapsed;
    }

    auto MeditationTimer::isStopped() const noexcept -> bool
    {
        return !isRunning;
    }

    auto MeditationTimer::intervalReached() const noexcept -> bool
    {
        return hasInterval && (elapsed.count() % intervalPeriod.count()) == 0;
    }

    auto MeditationTimer::calculatePercentageValue() const noexcept -> unsigned int
    {
        const auto percentage = static_cast<float>(elapsed.count()) / duration.count();
        return std::lround(percentage * 100);
    }

    void MeditationTimer::stop()
    {
        isRunning = false;
    }

    void MeditationTimer::setCounterVisible(bool isVisible) noexcept
    {
        timer->setVisible(isVisible);
    }

    void MeditationTimer::onInterval() const
    {
        AudioServiceAPI::PlaybackStart(application,
                                       audio::PlaybackType::Meditation,
                                       purefs::dir::getCurrentOSPath() / "assets/audio/meditation/gong.mp3");
    }

    void MeditationTimer::registerTimeoutCallback(const std::function<void()> &cb)
    {
        timeoutCallback = cb;
    }
} // namespace gui
