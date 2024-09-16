// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "PowerNapProgressWindow.hpp"
#include "ApplicationBellPowerNap.hpp"
#include "data/PowerNapStyle.hpp"
#include "data/PowerNapSwitchData.hpp"

#include <apps-common/widgets/ProgressTimerWithBarGraphAndCounter.hpp>
#include <apps-common/widgets/BarGraph.hpp>
#include <apps-common/widgets/TimeMinuteSecondWidget.hpp>
#include <common/widgets/BellStatusClock.hpp>
#include <gui/widgets/Icon.hpp>

#include <keymap/KeyMap.hpp>

namespace
{
    constexpr auto powerNapTimerName{"PowerNapTimer"};
    constexpr auto initialInterval{std::chrono::seconds{1}};
} // namespace

namespace gui
{
    PowerNapProgressWindow::PowerNapProgressWindow(
        app::ApplicationCommon *app,
        std::shared_ptr<app::powernap::PowerNapProgressContract::Presenter> &&windowPresenter)
        : AppWindow(app, window::name::powerNapProgress), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void PowerNapProgressWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        buildLayout();
        configureTimer();
    }

    void PowerNapProgressWindow::buildLayout()
    {
        using namespace powerNapStyle;
        const auto progressArcRadius = progressStyle::progress::radius;
        const auto progressArcWidth  = progressStyle::progress::penWidth;
        const auto arcStartAngle     = -90 - progressStyle::progress::verticalDeviationDegrees;
        const auto arcSweepAngle     = 360 - (2 * progressStyle::progress::verticalDeviationDegrees);
        const auto arcProgressSteps  = 1000;

        Arc::ShapeParams arcParams;
        arcParams.setCenterPoint(Point(getWidth() / 2, getHeight() / 2))
            .setRadius(progressArcRadius)
            .setStartAngle(arcStartAngle) // Start drawing the circle from top.
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
        clock->setMaximumSize(progressStyle::clock::maxSizeX, progressStyle::clock::maxSizeY);
        clock->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        clock->setMargins(Margins(0, progressStyle::clock::marginTop, 0, 0));

        timer = new TimeMinuteSecondWidget(mainVBox,
                                           0,
                                           0,
                                           progressStyle::timer::maxSizeX,
                                           progressStyle::timer::maxSizeY,
                                           TimeMinuteSecondWidget::DisplayType::OnlyMinutes);
        timer->setMinimumSize(progressStyle::timer::maxSizeX, progressStyle::timer::maxSizeY);
        timer->setMargins(Margins(0, progressStyle::timer::marginTop, 0, 0));
        timer->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        pauseBox = new VBox(mainVBox);
        pauseBox->setMinimumSize(progressStyle::pauseIcon::minSizeX, progressStyle::pauseIcon::minSizeY);
        pauseBox->setMargins(Margins(0, progressStyle::pauseIcon::marginTop, 0, 0));
        pauseBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        pauseBox->setEdges(RectangleEdge::None);
        new Image(pauseBox, progressStyle::pauseIcon::image, ImageTypeSpecifier::W_G);
        pauseBox->setVisible(false);
        pauseBox->resizeItems();

        ringBox = new VBox(mainVBox);
        ringBox->setMinimumSize(progressStyle::ringIcon::minSizeX, progressStyle::ringIcon::minSizeY);
        ringBox->setMargins(Margins(0, progressStyle::pauseIcon::marginTop, 0, 0));
        ringBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        ringBox->setEdges(RectangleEdge::None);
        new Image(ringBox, progressStyle::ringIcon::image, ImageTypeSpecifier::W_G);
        ringBox->setVisible(false);
        ringBox->resizeItems();

        bottomDescription = new TextFixedSize(
            mainVBox, 0, 0, progressStyle::bottomDescription::maxSizeX, progressStyle::bottomDescription::maxSizeY);
        bottomDescription->setMaximumSize(progressStyle::bottomDescription::maxSizeX,
                                          progressStyle::bottomDescription::maxSizeY);
        bottomDescription->setFont(progressStyle::bottomDescription::font);
        bottomDescription->setMargins(Margins(0, 0, 0, 0));
        bottomDescription->activeItem = false;
        bottomDescription->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        bottomDescription->setText(utils::translate("app_bellmain_power_nap"));
        bottomDescription->drawUnderline(false);
        bottomDescription->setVisible(true);

        mainVBox->resizeItems();
    }

    void PowerNapProgressWindow::configureTimer()
    {
        auto progressTimer = std::make_unique<app::ProgressTimerWithBarGraphAndCounter>(
            application, *this, powerNapTimerName, initialInterval, app::ProgressCountdownMode::Increasing);
        progressTimer->attach(progress);
        progressTimer->attach(timer);
        presenter->setTimer(std::move(progressTimer));
    }

    bool PowerNapProgressWindow::onInput(const InputEvent &inputEvent)
    {
        const auto key = mapKey(inputEvent.getKeyCode());

        /* Prevent leaving by back long press */
        if (inputEvent.isLongRelease() && (key == KeyMap::Back)) {
            return true;
        }

        if (inputEvent.isShortRelease()) {
            if (presenter->isNapFinished() && (key == KeyMap::LightPress)) {
                presenter->endNap();
                return true;
            }
            else if (!presenter->isNapFinished() && (key == KeyMap::LightPress)) {
                if (presenter->isTimerStopped()) {
                    presenter->resume();
                }
                else {
                    presenter->pause();
                }
                return true;
            }
            else if (!presenter->isNapFinished() && (key == KeyMap::Back)) {
                static_cast<app::Application *>(application)->resumeIdleTimer();
                presenter->abortNap();
                application->returnToPreviousWindow();
                return true;
            }
            return false;
        }
        return AppWindow::onInput(inputEvent);
    }

    void PowerNapProgressWindow::napEnded()
    {
        application->switchWindow(window::name::powerNapSessionEnded, std::make_unique<PowerNapSwitchData>());
    }

    void PowerNapProgressWindow::setTime(std::time_t newTime)
    {
        clock->setTime(newTime);
        clock->setTimeFormatSpinnerVisibility(true);
    }

    void PowerNapProgressWindow::setTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        clock->setTimeFormat(fmt);
    }

    RefreshModes PowerNapProgressWindow::updateTime()
    {
        if (presenter) {
            presenter->handleUpdateTimeEvent();
        }
        return RefreshModes::GUI_REFRESH_FAST;
    }

    void PowerNapProgressWindow::pause()
    {
        timer->setVisible(false);
        pauseBox->setVisible(true);
        mainVBox->resizeItems();
    }

    void PowerNapProgressWindow::resume()
    {
        timer->setVisible(true);
        pauseBox->setVisible(false);
        mainVBox->resizeItems();
    }

    void PowerNapProgressWindow::progressFinished()
    {
        timer->setVisible(false);
        pauseBox->setVisible(false);
        ringBox->setVisible(true);
        mainVBox->resizeItems();
    }

    void PowerNapProgressWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        AppWindow::onBeforeShow(mode, data);
        presenter->onBeforeShow();
        updateTime();

        if (mode == ShowMode::GUI_SHOW_INIT) {
            presenter->activate();
        }
    }
} // namespace gui
