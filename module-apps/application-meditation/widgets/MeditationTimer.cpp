// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationTimer.hpp"
#include "Style.hpp"

#include <GuiTimer.hpp>
#include <purefs/filesystem_paths.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <apps-common/widgets/ProgressTimerWithBarGraphAndCounter.hpp>

#include <gsl/assert>

namespace
{
    inline constexpr auto meditationTimerName = "MeditationTimer";
    inline constexpr std::chrono::seconds timerTick{1};
} // namespace
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

        text = new Text(progressBar, 0, 0, getWidth(), getHeight());
        text->setEdges(RectangleEdge::None);
        text->setFont(style::window::font::supersizemelight);
        text->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        text->setEditMode(EditMode::Browse);

        timer = std::make_unique<app::ProgressTimerWithBarGraphAndCounter>(
            application, *this, meditationTimerName, timerTick);
        timer->attach(progressBar);
        timer->attach(text);
        auto intervalCallback = [app = application] {
            AudioServiceAPI::PlaybackStart(app,
                                           audio::PlaybackType::Meditation,
                                           purefs::dir::getCurrentOSPath() / "assets/audio/meditation/gong.mp3");
        };
        timer->registerOnIntervalCallback(std::move(intervalCallback));
    }

    auto MeditationTimer::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool
    {
        setPosition(newDim.x, newDim.y);
        setSize(newDim.w, newDim.h);
        return true;
    }

    void MeditationTimer::setCounterVisible(bool isVisible) noexcept
    {
        text->setVisible(isVisible);
    }

    app::TimerWithCallbacks &MeditationTimer::getTimer() noexcept
    {
        Expects(timer != nullptr);
        return *timer;
    }
    gui::Progress &MeditationTimer::getProgress() noexcept
    {
        Expects(progressBar != nullptr);
        return *progressBar;
    }
} // namespace gui
