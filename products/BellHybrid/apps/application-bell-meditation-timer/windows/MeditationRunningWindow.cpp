// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationTimer.hpp"
#include "MeditationCommon.hpp"
#include "MeditationRunningWindow.hpp"
#include "MeditationStyle.hpp"

#include <audio/AudioMessage.hpp>
#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/widgets/ProgressTimerWithBarGraphAndCounter.hpp>
#include <purefs/filesystem_paths.hpp>

namespace
{
    inline constexpr auto meditationProgressTimerName = "MeditationProgressTimer";
    inline constexpr std::chrono::seconds baseTick{1};
    inline constexpr auto meditationProgressMode = app::ProgressCountdownMode::Increasing;
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
        const auto progressArcRadius = mrStyle::progress::radius;
        const auto progressArcWidth  = mrStyle::progress::penWidth;
        const auto arcStartAngle     = -90 - mrStyle::progress::verticalDeviationDegrees;
        const auto arcSweepAngle     = 360 - (2 * mrStyle::progress::verticalDeviationDegrees);
        const auto arcProgressSteps  = 1000;

        Arc::ShapeParams arcParams;
        arcParams.setCenterPoint(Point(getWidth() / 2, getHeight() / 2))
            .setRadius(progressArcRadius)
            .setStartAngle(arcStartAngle) // Start drawing the circle from top.
            .setSweepAngle(arcSweepAngle)
            .setPenWidth(progressArcWidth)
            .setBorderColor(ColorFullBlack);

        progress = new ArcProgressBar(this, arcParams, ArcProgressBar::ProgressDirection::CounterClockwise);
        progress->setMaximum(arcProgressSteps);

        mainVBox = new VBox(this, 0, 0, style::window_width, style::window_height);

        clock = new BellStatusClock(mainVBox);
        clock->setMaximumSize(mrStyle::clock::maxSizeX, mrStyle::clock::maxSizeY);
        clock->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        clock->setMargins(gui::Margins(0, mrStyle::clock::marginTop, 0, 0));

        timer = new gui::Text(mainVBox, 0, 0, 0, 0);
        timer->setFont(style::window::font::supersizeme);
        timer->setMinimumSize(mrStyle::timer::maxSizeX, mrStyle::timer::maxSizeY);
        timer->setMargins(gui::Margins(0, mrStyle::timer::marginTop, 0, 0));
        timer->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        icon = new Icon(mainVBox, 0, 0, 0, 0, {}, {});
        icon->setMinimumSize(mrStyle::pauseIcon::maxSizeX, mrStyle::pauseIcon::maxSizeY);
        icon->setMargins(gui::Margins(0, mrStyle::pauseIcon::marginTop, 0, 0));
        icon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        icon->image->set(mrStyle::pauseIcon::image, ImageTypeSpecifier::W_G);
        icon->setVisible(false);

        mainVBox->resizeItems();
    }

    void MeditationRunningWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        AppWindow::onBeforeShow(mode, data);
        presenter->onBeforeShow();
        updateTime();

        if (mode == ShowMode::GUI_SHOW_INIT) {
            playGong();
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
            reinterpret_cast<app::Application *>(application)->resumeIdleTimer();
            presenter->finish();
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
            application, *this, meditationProgressTimerName, baseTick, meditationProgressMode);
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
        playGong();
    }

    void MeditationRunningWindow::endSession()
    {
        playGong();
    }

    void MeditationRunningWindow::playGong()
    {
        auto msg = std::make_shared<service::AudioStartPlaybackRequest>(app::meditation::getMeditationAudioPath(),
                                                                        audio::PlaybackType::Meditation);
        application->bus.sendUnicast(std::move(msg), service::audioServiceName);
    }
} // namespace gui
