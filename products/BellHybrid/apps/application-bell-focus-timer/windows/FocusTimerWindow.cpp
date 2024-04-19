// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FocusTimerWindow.hpp"
#include <data/FocusTimerStyle.hpp>

#include <Application.hpp>
#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <apps-common/widgets/ProgressTimerWithBarGraphAndCounter.hpp>
#include <module-gui/gui/input/InputEvent.hpp>

namespace
{
    constexpr auto progressTimerName{"FocusProgressTimer"};
    constexpr auto progressTimerPeriod{std::chrono::seconds{1}};
    constexpr auto progressMode{app::ProgressCountdownMode::Increasing};
    constexpr auto focusTimerText{"Focus timer"};
    constexpr auto timeToFocusText{"Time to focus"};
    constexpr auto shortBreakTimeText{"Short break"};
    constexpr auto timeForBreakText{"Time for a break"};
    constexpr auto endOfAllSessionText{"Well done!"};
} // namespace

namespace app::focus
{
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

        gui::Arc::ShapeParams arcParams;
        arcParams.setCenterPoint(gui::Point(getWidth() / 2, getHeight() / 2))
            .setRadius(progressArcRadius)
            .setStartAngle(arcStartAngle)
            .setSweepAngle(arcSweepAngle)
            .setPenWidth(progressArcWidth)
            .setBorderColor(gui::ColorFullBlack);

        progress = new gui::ArcProgressBar(this,
                                           arcParams,
                                           gui::ArcProgressBar::ProgressDirection::CounterClockwise,
                                           gui::ArcProgressBar::ProgressChange::DecrementFromFull);
        progress->setMaximum(arcProgressSteps);

        mainVBox = new gui::VBox(this, 0, 0, style::window_width, style::window_height);

        clock = new gui::BellStatusClock(mainVBox);
        clock->setMaximumSize(runningStyle::clock::maxSizeX, runningStyle::clock::maxSizeY);
        clock->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        clock->setMargins(gui::Margins(0, runningStyle::clock::marginTop, 0, 0));

        timer = new gui::TimeMinuteSecondWidget(mainVBox,
                                                0,
                                                0,
                                                runningStyle::timer::maxSizeX,
                                                runningStyle::timer::maxSizeY,
                                                gui::TimeMinuteSecondWidget::DisplayType::MinutesThenSeconds);
        timer->setMinimumSize(runningStyle::timer::maxSizeX, runningStyle::timer::maxSizeY);
        timer->setMargins(gui::Margins(0, runningStyle::timer::marginTop, 0, 0));
        timer->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

        iconPause = new gui::Icon(mainVBox, 0, 0, 0, 0, {}, {});
        iconPause->setMinimumSize(runningStyle::pauseIcon::maxSizeX, runningStyle::pauseIcon::maxSizeY);
        iconPause->setMargins(gui::Margins(0, runningStyle::pauseIcon::marginTop, 0, 0));
        iconPause->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        iconPause->image->set(runningStyle::pauseIcon::image, gui::ImageTypeSpecifier::W_G);
        iconPause->setVisible(false);

        iconRing = new gui::Icon(mainVBox, 0, 0, 0, 0, {}, {});
        iconRing->setMinimumSize(runningStyle::ringIcon::maxSizeX, runningStyle::ringIcon::maxSizeY);
        iconRing->setMargins(gui::Margins(0, runningStyle::ringIcon::marginTop, 0, 0));
        iconRing->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        iconRing->image->set(runningStyle::ringIcon::image, gui::ImageTypeSpecifier::W_G);
        iconRing->setVisible(false);

        bottomDescription = new gui::TextFixedSize(mainVBox, 0, 0, 250, 80);
        bottomDescription->setMaximumSize(runningStyle::bottomDescription::maxSizeX,
                                          runningStyle::bottomDescription::maxSizeY);
        bottomDescription->setFont(runningStyle::bottomDescription::font);
        bottomDescription->setMargins(gui::Margins(0, 0, 0, 0));
        bottomDescription->activeItem = false;
        bottomDescription->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        bottomDescription->setRichText(focusTimerText);
        bottomDescription->drawUnderline(false);
        bottomDescription->setVisible(true);

        mainVBox->resizeItems();
    }

    void FocusTimerWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        AppWindow::onBeforeShow(mode, data);
        presenter->onBeforeShow();
        updateTime();
        if (mode == gui::ShowMode::GUI_SHOW_INIT) {
            presenter->playGong();
            presenter->start();
        }
    }

    bool FocusTimerWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
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
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_RF)) {
            static_cast<app::Application *>(application)->resumeIdleTimer();
            presenter->abandon();
            return true;
        }

        return AppWindow::onInput(inputEvent);
    }

    void FocusTimerWindow::showEndOfAllSessionsInfo()
    {
        timer->setVisible(false);
        iconPause->setVisible(false);
        iconRing->setVisible(true);
        bottomDescription->setVisible(true);
        bottomDescription->setText(endOfAllSessionText);
        mainVBox->resizeItems();
        presenter->playGong();
    }

    void FocusTimerWindow::showFocusSessionCountdown()
    {
        timer->setVisible(true);
        iconPause->setVisible(false);
        iconRing->setVisible(false);
        bottomDescription->setVisible(true);
        bottomDescription->setText(focusTimerText);
        mainVBox->resizeItems();
    }

    void FocusTimerWindow::showTimeForBreakInfo()
    {
        timer->setVisible(false);
        iconPause->setVisible(false);
        iconRing->setVisible(true);
        bottomDescription->setVisible(true);
        bottomDescription->setText(timeForBreakText);
        mainVBox->resizeItems();
        presenter->playGong();
    }

    void FocusTimerWindow::showShortBreakCountdown()
    {
        timer->setVisible(true);
        iconPause->setVisible(false);
        iconRing->setVisible(false);
        bottomDescription->setVisible(true);
        bottomDescription->setText(shortBreakTimeText);
        mainVBox->resizeItems();
    }

    void FocusTimerWindow::showLongBreakCountdown()
    {
        timer->setVisible(true);
        iconPause->setVisible(false);
        iconRing->setVisible(false);
        bottomDescription->setVisible(true);
        bottomDescription->setText(utils::translate("app_bell_focus_long_break"));
        mainVBox->resizeItems();
    }

    void FocusTimerWindow::showTimeForFocusInfo()
    {
        timer->setVisible(false);
        iconPause->setVisible(false);
        iconRing->setVisible(true);
        bottomDescription->setVisible(true);
        bottomDescription->setText(timeToFocusText);
        mainVBox->resizeItems();
        presenter->playGong();
    }

    void FocusTimerWindow::pause()
    {
        timer->setVisible(false);
        iconPause->setVisible(true);
        iconRing->setVisible(false);
        mainVBox->resizeItems();
    }

    void FocusTimerWindow::resume()
    {
        timer->setVisible(true);
        iconPause->setVisible(false);
        iconRing->setVisible(false);
        mainVBox->resizeItems();
    }

    void FocusTimerWindow::configureTimer()
    {
        auto progressTimer = std::make_unique<app::ProgressTimerWithBarGraphAndCounter>(
            application, *this, progressTimerName, progressTimerPeriod, progressMode);
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

    gui::RefreshModes FocusTimerWindow::updateTime()
    {
        if (presenter != nullptr) {
            presenter->handleUpdateTimeEvent();
        }
        return gui::RefreshModes::GUI_REFRESH_FAST;
    }
} // namespace app::focus
