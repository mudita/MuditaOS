// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Arc.hpp>

#include "MeditationTimer.hpp"
#include "MeditationCommon.hpp"
#include "MeditationCountdownWindow.hpp"
#include "MeditationStyle.hpp"

#include <apps-common/widgets/ProgressTimerWithBarGraphAndCounter.hpp>

namespace
{
    inline constexpr auto meditationCountdownTimerName = "MeditationCountdownTimer";
    inline constexpr std::chrono::seconds baseTick{1};
    inline constexpr auto meditationCountdownMode = app::ProgressCountdownMode::Increasing;
} // namespace

namespace gui
{
    MeditationCountdownWindow::MeditationCountdownWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::meditation::MeditationCountdownContract::Presenter> &&windowPresenter)
        : AppWindow(app, app::meditation::windows::meditationCountdown), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void MeditationCountdownWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        buildLayout();
        configureTimer();
    }

    void MeditationCountdownWindow::buildLayout()
    {
        using namespace app::meditationStyle;
        const auto progressArcRadius = mcStyle::progress::radius;
        const auto progressArcWidth  = mcStyle::progress::penWidth;
        const auto arcStartAngle     = -90 + mcStyle::progress::verticalDeviationDegrees;
        const auto arcSweepAngle     = 360 - (2 * mcStyle::progress::verticalDeviationDegrees);

        Arc::ShapeParams arcParams;
        arcParams.setCenterPoint(Point(getWidth() / 2, getHeight() / 2))
            .setRadius(progressArcRadius)
            .setStartAngle(arcStartAngle)
            .setSweepAngle(arcSweepAngle)
            .setPenWidth(progressArcWidth)
            .setBorderColor(ColorFullBlack);

        progress = new Arc(this, arcParams);

        mainVBox = new VBox(this, 0, 0, style::window_width, style::window_height);

        description = new Text(mainVBox, 0, 0, 0, 0);
        description->setText(utils::translate("app_meditation_countdown_desc"));
        description->setMaximumSize(mcStyle::description::maxSizeX, mcStyle::description::maxSizeY);
        description->setFont(mcStyle::description::font);
        description->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        description->setMargins(gui::Margins(0, mcStyle::description::marginTop, 0, 0));

        timer = new gui::TimeFixedWidget(mainVBox, 0, 0, mrStyle::timer::maxSizeX, mrStyle::timer::maxSizeY, true);
        timer->setFont(mcStyle::timer::font);
        timer->setMinimumSize(mcStyle::timer::maxSizeX, mcStyle::timer::maxSizeY);
        timer->setMargins(gui::Margins(0, mcStyle::timer::marginTop, 0, 0));
        timer->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        mainVBox->resizeItems();
    }

    void MeditationCountdownWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        AppWindow::onBeforeShow(mode, data);

        if (mode == ShowMode::GUI_SHOW_INIT) {
            presenter->start();
        }
    }

    bool MeditationCountdownWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_RF)) {
            presenter->stop();
        }
        return AppWindow::onInput(inputEvent);
    }

    void MeditationCountdownWindow::configureTimer()
    {
        auto progressTimer = std::make_unique<app::ProgressTimerWithBarGraphAndCounter>(
            application, *this, meditationCountdownTimerName, baseTick, meditationCountdownMode);
        progressTimer->attach(timer);
        presenter->setTimer(std::move(progressTimer));
    }

} // namespace gui
