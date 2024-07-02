// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationTimer.hpp"
#include "MeditationCommon.hpp"
#include "MeditationRunningWindow.hpp"
#include "MeditationStyle.hpp"

#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/widgets/ProgressTimerWithBarGraphAndCounter.hpp>
#include <apps-common/widgets/BarGraph.hpp>
#include <common/widgets/BellStatusClock.hpp>
#include <gui/widgets/Icon.hpp>

namespace
{
    constexpr auto meditationProgressTimerName{"MeditationProgressTimer"};
    constexpr std::chrono::seconds initialInterval{1};
    constexpr auto meditationProgressMode{app::ProgressCountdownMode::Increasing};
} // namespace

namespace gui
{
    MeditationRunningWindow::MeditationRunningWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::meditation::MeditationProgressContract::Presenter> &&windowPresenter)
        : AppWindow(app, app::meditation::windows::meditationProgress), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void MeditationRunningWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        buildLayout();
        configureTimer();
    }

    void MeditationRunningWindow::buildLayout()
    {
        using namespace app::meditationStyle;
        const auto progressArcRadius = runningStyle::progress::radius;
        const auto progressArcWidth  = runningStyle::progress::penWidth;
        const auto arcStartAngle =
            -90 - runningStyle::progress::verticalDeviationDegrees; // -90 to start drawing the circle from top
        const auto arcSweepAngle    = 360 - (2 * runningStyle::progress::verticalDeviationDegrees);
        const auto arcProgressSteps = 1000;

        Arc::ShapeParams arcParams;
        arcParams.setCenterPoint(Point(getWidth() / 2, getHeight() / 2))
            .setRadius(progressArcRadius)
            .setStartAngle(arcStartAngle)
            .setSweepAngle(arcSweepAngle)
            .setPenWidth(progressArcWidth)
            .setBorderColor(ColorFullBlack);

        progress = new ArcProgressBar(this,
                                      arcParams,
                                      ArcProgressBar::ProgressDirection::CounterClockwise,
                                      ArcProgressBar::ProgressChange::DecrementFromFull);
        progress->setMaximum(arcProgressSteps);

        mainVBox = new VBox(this, 0, 0, style::window_width, style::window_height);

        clock = new BellStatusClock(mainVBox);
        clock->setMaximumSize(runningStyle::clock::maxSizeX, runningStyle::clock::maxSizeY);
        clock->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        clock->setMargins(gui::Margins(0, runningStyle::clock::marginTop, 0, 0));

        timer = new gui::TimeMinuteSecondWidget(mainVBox,
                                                0,
                                                0,
                                                runningStyle::timer::maxSizeX,
                                                runningStyle::timer::maxSizeY,
                                                gui::TimeMinuteSecondWidget::DisplayType::OnlyMinutes);
        timer->setMinimumSize(runningStyle::timer::maxSizeX, runningStyle::timer::maxSizeY);
        timer->setMargins(gui::Margins(0, runningStyle::timer::marginTop, 0, 0));
        timer->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        icon = new Icon(mainVBox, 0, 0, 0, 0, {}, {});
        icon->setMinimumSize(runningStyle::pauseIcon::maxSizeX, runningStyle::pauseIcon::maxSizeY);
        icon->setMargins(gui::Margins(0, runningStyle::pauseIcon::marginTop, 0, 0));
        icon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        icon->image->set(runningStyle::pauseIcon::image, ImageTypeSpecifier::W_G);
        icon->setVisible(false);

        bottomDescription = new gui::TextFixedSize(
            mainVBox, 0, 0, runningStyle::bottomDescription::maxSizeX, runningStyle::bottomDescription::maxSizeY);
        bottomDescription->setMaximumSize(runningStyle::bottomDescription::maxSizeX,
                                          runningStyle::bottomDescription::maxSizeY);
        bottomDescription->setFont(runningStyle::bottomDescription::font);
        bottomDescription->setMargins(gui::Margins(0, 0, 0, 0));
        bottomDescription->activeItem = false;
        bottomDescription->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        bottomDescription->setRichText(utils::translate("app_bellmain_meditation_timer"));
        bottomDescription->drawUnderline(false);
        bottomDescription->setVisible(true);

        mainVBox->resizeItems();
    }

    void MeditationRunningWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        AppWindow::onBeforeShow(mode, data);
        presenter->onBeforeShow();
        updateTime();

        if (mode == ShowMode::GUI_SHOW_INIT) {
            presenter->playGongSound();
            presenter->start();
        }
    }

    bool MeditationRunningWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            if (presenter->isTimerStopped()) {
                presenter->resume();
            }
            else {
                presenter->pause();
            }
            return true;
        }
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_RF)) {
            static_cast<app::Application *>(application)->resumeIdleTimer();
            presenter->abandon();
            return true;
        }

        return AppWindow::onInput(inputEvent);
    }

    void MeditationRunningWindow::progressFinished()
    {
        endSession();
    }

    void MeditationRunningWindow::intervalReached()
    {
        intervalTimeout();
    }

    void MeditationRunningWindow::pause()
    {
        timer->setVisible(false);
        icon->setVisible(true);
        mainVBox->resizeItems();
    }

    void MeditationRunningWindow::resume()
    {
        timer->setVisible(true);
        icon->setVisible(false);
        mainVBox->resizeItems();
    }

    void MeditationRunningWindow::configureTimer()
    {
        auto progressTimer = std::make_unique<app::ProgressTimerWithBarGraphAndCounter>(
            application, *this, meditationProgressTimerName, initialInterval, meditationProgressMode);
        progressTimer->attach(progress);
        progressTimer->attach(timer);
        presenter->setTimer(std::move(progressTimer));
    }

    void MeditationRunningWindow::setTime(std::time_t newTime)
    {
        clock->setTime(newTime);
        clock->setTimeFormatSpinnerVisibility(true);
    }

    void MeditationRunningWindow::setTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        clock->setTimeFormat(fmt);
    }

    RefreshModes MeditationRunningWindow::updateTime()
    {
        if (presenter != nullptr) {
            presenter->handleUpdateTimeEvent();
        }
        return RefreshModes::GUI_REFRESH_FAST;
    }

    void MeditationRunningWindow::intervalTimeout()
    {
        presenter->playGongSound();
    }

    void MeditationRunningWindow::endSession()
    {
        presenter->playEndSound();
    }
} // namespace gui
