#include "MeditationTimer.hpp"

#include <cassert>

#include <application-meditation/data/Style.hpp>
#include <time/time_conversion.hpp>

#include "GuiTimer.hpp"

namespace gui
{
    namespace
    {
        constexpr gui::ms TimerInterval{1000};
    } // namespace

    MeditationTimer::MeditationTimer(std::uint32_t x,
                                     std::uint32_t y,
                                     std::uint32_t width,
                                     std::uint32_t height,
                                     app::ApplicationMeditation *app,
                                     Item *_parent)
        : HBox(_parent, x, y, width, height), application{app}
    {
        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
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
        timer->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        timer->setFont(style::window::font::supersizemelight);
        timer->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        timer->setEditMode(EditMode::BROWSE);

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

    void MeditationTimer::reset(std::chrono::seconds _duration) noexcept
    {
        assert(_duration != std::chrono::seconds::zero()); // Pre-condition check.

        duration = _duration;
        elapsed  = std::chrono::seconds::zero();
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
        auto timerTask =
            std::make_unique<app::GuiTimer>("MeditationTimer", application, TimerInterval, Timer::Type::Continous);
        timerCallback = [this](Item &it, Timer &timerTask) { return onTimerTimeout(it, timerTask); };
        timerTask->start();
        application->connect(std::move(timerTask), this);
    }

    auto MeditationTimer::onTimerTimeout(Item &self, Timer &timerTask) -> bool
    {
        if (isFinished() || isStopped()) {
            timerTask.stop();
            detachTimer(timerTask);

            if (isFinished() && timeoutCallback != nullptr) {
                timeoutCallback();
            }

            return true;
        }

        ++elapsed;
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
    auto MeditationTimer::calculatePercentageValue() const noexcept -> unsigned int
    {
        const auto percentage = static_cast<float>(elapsed.count()) / duration.count();
        return std::lround(percentage * 100);
    }

    void MeditationTimer::stop()
    {
        isRunning = false;
    }

    void MeditationTimer::setTimerVisible(bool isVisible) const noexcept
    {
        timer->setVisible(isVisible);
    }

    void MeditationTimer::registerTimeoutCallback(const std::function<void()> &cb)
    {
        timeoutCallback = cb;
    }
} // namespace gui
