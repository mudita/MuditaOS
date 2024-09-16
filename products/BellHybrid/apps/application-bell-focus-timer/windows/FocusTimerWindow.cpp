// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "FocusTimerWindow.hpp"
#include <data/FocusTimerStyle.hpp>

#include <Application.hpp>

#include <common/widgets/BellStatusClock.hpp>
#include <apps-common/widgets/BarGraph.hpp>
#include <apps-common/widgets/TimeMinuteSecondWidget.hpp>
#include <apps-common/widgets/ProgressTimerWithBarGraphAndCounter.hpp>

namespace
{
    constexpr auto progressTimerName{"FocusProgressTimer"};
    constexpr auto initialInterval{std::chrono::seconds{1}};
    constexpr auto progressMode{app::ProgressCountdownMode::Increasing};
} // namespace

namespace app::focus
{
    using namespace gui;

    FocusTimerWindow::FocusTimerWindow(app::ApplicationCommon *app,
                                       std::unique_ptr<FocusTimerContract::Presenter> &&windowPresenter,
                                       const std::string &name)
        : AppWindow(app, name), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void FocusTimerWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        buildLayout();
        configureTimer();
    }

    void FocusTimerWindow::buildLayout()
    {
        using namespace app::focusTimerStyle;
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
        mainVBox->setEdges(RectangleEdge::None);

        clock = new BellStatusClock(mainVBox);
        clock->setMaximumSize(runningStyle::clock::maxSizeX, runningStyle::clock::maxSizeY);
        clock->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        clock->setMargins(Margins(0, runningStyle::clock::marginTop, 0, 0));

        timer = new TimeMinuteSecondWidget(mainVBox,
                                           0,
                                           0,
                                           runningStyle::timer::maxSizeX,
                                           runningStyle::timer::maxSizeY,
                                           TimeMinuteSecondWidget::DisplayType::MinutesThenSeconds);
        timer->setMinimumSize(runningStyle::timer::maxSizeX, runningStyle::timer::maxSizeY);
        timer->setMargins(Margins(0, runningStyle::timer::marginTop, 0, 0));
        timer->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        pauseBox = new VBox(mainVBox);
        pauseBox->setMinimumSize(runningStyle::pauseIcon::minSizeX, runningStyle::pauseIcon::minSizeY);
        pauseBox->setMargins(Margins(0, runningStyle::pauseIcon::marginTop, 0, 0));
        pauseBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        pauseBox->setEdges(RectangleEdge::None);
        new Image(pauseBox, runningStyle::pauseIcon::image, ImageTypeSpecifier::W_G);
        pauseBox->setVisible(false);
        pauseBox->resizeItems();

        ringBox = new VBox(mainVBox);
        ringBox->setMinimumSize(runningStyle::ringIcon::minSizeX, runningStyle::ringIcon::minSizeY);
        ringBox->setMargins(Margins(0, runningStyle::ringIcon::marginTop, 0, 0));
        ringBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        ringBox->setEdges(RectangleEdge::None);
        new Image(ringBox, runningStyle::ringIcon::image, ImageTypeSpecifier::W_G);
        ringBox->setVisible(false);
        ringBox->resizeItems();

        bottomDescription = new TextFixedSize(
            mainVBox, 0, 0, runningStyle::bottomDescription::maxSizeX, runningStyle::bottomDescription::maxSizeY);
        bottomDescription->setMaximumSize(runningStyle::bottomDescription::maxSizeX,
                                          runningStyle::bottomDescription::maxSizeY);
        bottomDescription->setFont(runningStyle::bottomDescription::font);
        bottomDescription->setMargins(Margins(0, 0, 0, 0));
        bottomDescription->activeItem = false;
        bottomDescription->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        bottomDescription->setRichText(utils::translate("app_bell_focus_time"));
        bottomDescription->drawUnderline(false);
        bottomDescription->setVisible(true);

        mainVBox->resizeItems();
    }

    void FocusTimerWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        AppWindow::onBeforeShow(mode, data);
        presenter->onBeforeShow();
        updateTime();
        if (mode == ShowMode::GUI_SHOW_INIT) {
            presenter->playGong();
            presenter->start();
        }
    }

    bool FocusTimerWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            if (presenter->isAllSessionsFinished()) {
                presenter->finish();
            }
            if (presenter->isTimerStopped()) {
                presenter->resume();
            }
            else {
                presenter->pause();
            }
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            static_cast<app::Application *>(application)->resumeIdleTimer();
            presenter->abandon();
            return true;
        }

        return AppWindow::onInput(inputEvent);
    }

    void FocusTimerWindow::showEndOfAllSessionsInfo()
    {
        timer->setVisible(false);
        pauseBox->setVisible(false);
        ringBox->setVisible(true);
        bottomDescription->setVisible(true);
        bottomDescription->setText(utils::translate("app_bell_focus_well_done"));
        mainVBox->resizeItems();
        presenter->playGong();
    }

    void FocusTimerWindow::showFocusSessionCountdown()
    {
        timer->setVisible(true);
        pauseBox->setVisible(false);
        ringBox->setVisible(false);
        bottomDescription->setVisible(true);
        bottomDescription->setText(utils::translate("app_bell_focus_time"));
        mainVBox->resizeItems();
    }

    void FocusTimerWindow::showTimeForBreakInfo()
    {
        timer->setVisible(false);
        pauseBox->setVisible(false);
        ringBox->setVisible(true);
        bottomDescription->setVisible(true);
        bottomDescription->setText(utils::translate("app_bell_focus_time_for_break"));
        mainVBox->resizeItems();
        presenter->playGong();
    }

    void FocusTimerWindow::showShortBreakCountdown()
    {
        timer->setVisible(true);
        pauseBox->setVisible(false);
        ringBox->setVisible(false);
        bottomDescription->setVisible(true);
        bottomDescription->setText(utils::translate("app_bell_focus_short_break"));
        mainVBox->resizeItems();
    }

    void FocusTimerWindow::showLongBreakCountdown()
    {
        timer->setVisible(true);
        pauseBox->setVisible(false);
        ringBox->setVisible(false);
        bottomDescription->setVisible(true);
        bottomDescription->setText(utils::translate("app_bell_focus_long_break"));
        mainVBox->resizeItems();
    }

    void FocusTimerWindow::showTimeForFocusInfo()
    {
        timer->setVisible(false);
        pauseBox->setVisible(false);
        ringBox->setVisible(true);
        bottomDescription->setVisible(true);
        bottomDescription->setText(utils::translate("app_bell_focus_time_for_focus"));
        mainVBox->resizeItems();
        presenter->playGong();
    }

    void FocusTimerWindow::pause()
    {
        timer->setVisible(false);
        pauseBox->setVisible(true);
        ringBox->setVisible(false);
        mainVBox->resizeItems();
    }

    void FocusTimerWindow::resume()
    {
        timer->setVisible(true);
        pauseBox->setVisible(false);
        ringBox->setVisible(false);
        mainVBox->resizeItems();
    }

    void FocusTimerWindow::configureTimer()
    {
        auto progressTimer = std::make_unique<app::ProgressTimerWithBarGraphAndCounter>(
            application, *this, progressTimerName, initialInterval, progressMode);
        progressTimer->attach(progress);
        progressTimer->attach(timer);
        presenter->setTimer(std::move(progressTimer));
    }

    void FocusTimerWindow::setTime(std::time_t newTime)
    {
        clock->setTime(newTime);
        clock->setTimeFormatSpinnerVisibility(true);
    }

    void FocusTimerWindow::setTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        clock->setTimeFormat(fmt);
    }

    RefreshModes FocusTimerWindow::updateTime()
    {
        if (presenter != nullptr) {
            presenter->handleUpdateTimeEvent();
        }
        return RefreshModes::GUI_REFRESH_FAST;
    }
} // namespace app::focus
