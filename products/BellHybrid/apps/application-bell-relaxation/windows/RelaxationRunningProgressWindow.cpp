// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationRunningProgressWindow.hpp"
#include <data/RelaxationStyle.hpp>
#include <data/RelaxationSwitchData.hpp>
#include <common/BellCommonNames.hpp>
#include <popups/data/AudioErrorParams.hpp>
#include <ApplicationBellRelaxation.hpp>
#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/widgets/ProgressTimerWithBarGraphAndCounter.hpp>

namespace
{
    constexpr auto relaxationProgressTimerName{"RelaxationProgressTimer"};
    constexpr std::chrono::seconds relaxationProgressTimerPeriod{1};
    constexpr auto relaxationProgressMode{app::ProgressCountdownMode::Increasing};
} // namespace

namespace gui
{
    RelaxationRunningProgressWindow::RelaxationRunningProgressWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::relaxation::RelaxationRunningProgressContract::Presenter> &&presenter)
        : AppWindow(app, gui::window::name::relaxationRunningProgress), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void RelaxationRunningProgressWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        buildLayout();
        configureTimer();
    }

    void RelaxationRunningProgressWindow::buildLayout()
    {
        using namespace gui::relaxationStyle;
        const auto progressArcRadius = relStyle::progressTime::radius;
        const auto progressArcWidth  = relStyle::progressTime::penWidth;
        const auto arcStartAngle     = -90 - relStyle::progressTime::verticalDeviationDegrees;
        const auto arcSweepAngle     = 360 - (2 * relStyle::progressTime::verticalDeviationDegrees);
        const auto arcProgressSteps  = 1000;

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
        clock->setMaximumSize(relStyle::clock::maxSizeX, relStyle::clock::maxSizeY);
        clock->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        clock->setMargins(Margins(0, relStyle::clock::marginTop, 0, 0));

        timer = new TimeMinuteSecondWidget(mainVBox,
                                           0,
                                           0,
                                           relStyle::timer::maxSizeX,
                                           relStyle::timer::maxSizeY,
                                           TimeMinuteSecondWidget::DisplayType::OnlyMinutes);
        timer->setMinimumSize(relStyle::timer::maxSizeX, relStyle::timer::maxSizeY);
        timer->setMargins(Margins(0, relStyle::timer::marginTop, 0, 0));
        timer->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        icon = new Icon(mainVBox, 0, 0, 0, 0, {}, {});
        icon->setMinimumSize(relStyle::pauseIcon::maxSizeX, relStyle::pauseIcon::maxSizeY);
        icon->setMargins(gui::Margins(0, relStyle::pauseIcon::marginTop, 0, 0));
        icon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        icon->image->set(relStyle::pauseIcon::image, ImageTypeSpecifier::W_G);
        icon->setVisible(false);

        mainVBox->resizeItems();
    }

    void RelaxationRunningProgressWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        presenter->onBeforeShow();
        updateTime();

        if (data && typeid(*data) == typeid(RelaxationSwitchData)) {
            auto *audioSwitchData = static_cast<RelaxationSwitchData *>(data);
            audioContext          = audioSwitchData->getAudioContext();
            presenter->activate(audioContext->getSound());
        }
    }

    bool RelaxationRunningProgressWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease()) {
            if (inputEvent.is(KeyCode::KEY_RF)) {
                presenter->stop();
                return true;
            }
            else if (inputEvent.is(KeyCode::KEY_ENTER)) {
                if (presenter->isTimerStopped()) {
                    presenter->resume();
                }
                else {
                    presenter->pause();
                }
                return true;
            }
            else if (inputEvent.is(KeyCode::KEY_DOWN) || inputEvent.is(KeyCode::KEY_UP)) {
                application->switchWindow(gui::popup::window::volume_window);
                return true;
            }
        }
        return AppWindow::onInput(inputEvent);
    }

    void RelaxationRunningProgressWindow::onFinished()
    {
        if (presenter->isTimerFinished()) {
            application->switchWindow(gui::window::name::relaxationEnded,
                                      std::make_unique<gui::RelaxationEndedSwitchData>());
        }
        else {
            application->returnToPreviousWindow();
        }
    }

    void RelaxationRunningProgressWindow::onPaused()
    {
        timer->setVisible(false);
        icon->setVisible(true);
        mainVBox->resizeItems();
    }

    void RelaxationRunningProgressWindow::resume()
    {
        timer->setVisible(true);
        icon->setVisible(false);
        mainVBox->resizeItems();
    }

    void RelaxationRunningProgressWindow::configureTimer()
    {
        auto progressTimer = std::make_unique<app::ProgressTimerWithBarGraphAndCounter>(
            application, *this, relaxationProgressTimerName, relaxationProgressTimerPeriod, relaxationProgressMode);
        progressTimer->attach(progress);
        progressTimer->attach(timer);
        presenter->setTimer(std::move(progressTimer));
    }

    void RelaxationRunningProgressWindow::setTime(std::time_t newTime)
    {
        clock->setTime(newTime);
        clock->setTimeFormatSpinnerVisibility(true);
    }

    void RelaxationRunningProgressWindow::setTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        clock->setTimeFormat(fmt);
    }

    RefreshModes RelaxationRunningProgressWindow::updateTime()
    {
        if (presenter != nullptr) {
            presenter->handleUpdateTimeEvent();
        }
        return RefreshModes::GUI_REFRESH_FAST;
    }

    void RelaxationRunningProgressWindow::handleError()
    {
        auto switchData = std::make_unique<AudioErrorParams>(AudioErrorType::UnsupportedMediaType);
        application->switchWindow(gui::window::name::audioErrorWindow, std::move(switchData));
    }

    void RelaxationRunningProgressWindow::handleDeletedFile()
    {
        auto switchData = std::make_unique<AudioErrorParams>(AudioErrorType::FileDeleted);
        application->switchWindow(gui::window::name::audioErrorWindow, std::move(switchData));
    }
} // namespace gui
