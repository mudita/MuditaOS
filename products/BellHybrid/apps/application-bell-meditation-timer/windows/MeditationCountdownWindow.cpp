// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MeditationTimer.hpp"
#include "MeditationCommon.hpp"
#include "MeditationCountdownWindow.hpp"
#include "MeditationStyle.hpp"

#include <Arc.hpp>
#include <Text.hpp>
#include <apps-common/widgets/TimeMinuteSecondWidget.hpp>
#include <apps-common/widgets/ProgressTimerWithBarGraphAndCounter.hpp>

#include <keymap/KeyMap.hpp>

namespace
{
    constexpr auto meditationCountdownTimerName{"MeditationCountdownTimer"};
    constexpr std::chrono::seconds initialInterval{1};
    constexpr auto meditationCountdownMode{app::ProgressCountdownMode::Increasing};
    constexpr auto leftBoxSize{1};
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
        const auto progressArcRadius = countdownStyle::progress::radius;
        const auto progressArcWidth  = countdownStyle::progress::penWidth;
        const auto arcStartAngle     = -90 + countdownStyle::progress::verticalDeviationDegrees;
        const auto arcSweepAngle     = 360 - (2 * countdownStyle::progress::verticalDeviationDegrees);

        Arc::ShapeParams arcParams;
        arcParams.setCenterPoint(Point(getWidth() / 2, getHeight() / 2))
            .setRadius(progressArcRadius)
            .setStartAngle(arcStartAngle)
            .setSweepAngle(arcSweepAngle)
            .setPenWidth(progressArcWidth)
            .setBorderColor(ColorFullBlack);

        progress = new Arc(this, arcParams);

        mainVBox = new VBox(this, 0, 0, style::window_width, style::window_height);
        mainVBox->setEdges(RectangleEdge::None);

        description = new Text(mainVBox, 0, 0, 0, 0);
        description->setText(utils::translate("app_meditation_countdown_desc"));
        description->setMaximumSize(countdownStyle::description::maxSizeX, countdownStyle::description::maxSizeY);
        description->setFont(countdownStyle::description::font);
        description->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        description->setMargins(Margins(0, countdownStyle::description::marginTop, 0, 0));

        timer = new TimeMinuteSecondWidget(mainVBox,
                                           0,
                                           0,
                                           countdownStyle::timer::maxSizeX,
                                           countdownStyle::timer::maxSizeY,
                                           TimeMinuteSecondWidget::DisplayType::OnlySeconds);
        timer->setMinimumSize(countdownStyle::timer::maxSizeX, countdownStyle::timer::maxSizeY);
        timer->setMargins(Margins(0, countdownStyle::timer::marginTop, 0, 0));
        timer->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        bottomDescription = new TextFixedSize(
            mainVBox, 0, 0, countdownStyle::bottomDescription::maxSizeX, countdownStyle::bottomDescription::maxSizeY);
        bottomDescription->setMaximumSize(countdownStyle::bottomDescription::maxSizeX,
                                          countdownStyle::bottomDescription::maxSizeY);
        bottomDescription->setFont(countdownStyle::bottomDescription::font);
        bottomDescription->setMargins(Margins(0, 0, 0, 0));
        bottomDescription->activeItem = false;
        bottomDescription->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        bottomDescription->setRichText(utils::translate("app_bellmain_meditation_timer"));
        bottomDescription->drawUnderline(false);
        bottomDescription->setVisible(true);

        mainVBox->resizeItems();
    }

    void MeditationCountdownWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        AppWindow::onBeforeShow(mode, data);

        if (mode == ShowMode::GUI_SHOW_INIT) {
            presenter->start();
        }
        else if (presenter->isFinished()) {
            application->switchWindow(app::meditation::windows::meditationProgress);
        }
        presenter->setReady(true);
    }

    bool MeditationCountdownWindow::onInput(const InputEvent &inputEvent)
    {
        const auto key = mapKey(inputEvent.getKeyCode());
        if (key == KeyMap::Back) {
            presenter->stop();
        }
        if (key == KeyMap::DeepPressUp || key == KeyMap::DeepPressDown) {
            presenter->setReady(false);
        }
        return AppWindow::onInput(inputEvent);
    }

    void MeditationCountdownWindow::configureTimer()
    {
        auto progressTimer = std::make_unique<app::ProgressTimerWithBarGraphAndCounter>(
            application, *this, meditationCountdownTimerName, initialInterval, meditationCountdownMode);
        progressTimer->attach(timer);
        presenter->setTimer(std::move(progressTimer));
    }
} // namespace gui
