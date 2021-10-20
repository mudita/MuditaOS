// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationBellMeditationTimer.hpp"
#include "MeditationRunningWindow.hpp"
#include "MeditationStyle.hpp"

#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/widgets/ProgressTimer.hpp>
#include <purefs/filesystem_paths.hpp>
#include <service-audio/AudioServiceAPI.hpp>
#include <service-time/api/TimeSettingsApi.hpp>
#include <service-time/ServiceTime.hpp>

#include <log/log.hpp>
#include <i18n/i18n.hpp>

namespace
{
    inline constexpr auto meditationProgressTimerName = "MeditationProgressTimer";
    inline constexpr std::chrono::seconds baseTick{1};
    inline constexpr auto meditationProgressMode = app::ProgressCountdownMode::Increasing;
    inline constexpr auto meditationAudioPath    = "assets/audio/bell/meditation/Meditation_Gong.mp3";

    using namespace app::meditationStyle;

    void decorateProgressItem(gui::Rect *item, gui::Alignment::Vertical alignment)
    {
        item->setEdges(gui::RectangleEdge::None);
        item->activeItem = false;
        item->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, alignment));
    }

    gui::Label *createTitle(gui::VBox *parent)
    {
        auto title = new gui::Label(parent, 0, 0, parent->getWidth(), parent->getHeight() / 2);
        title->setText(utils::translate("app_meditation_title_main"));
        title->setFont(mrStyle::title::font);
        decorateProgressItem(title, gui::Alignment::Vertical::Top);
        return title;
    }

    gui::HBarGraph *createProgress(gui::VBox *parent)
    {
        auto progressBox = new gui::HBox(parent, 0, 0, parent->getWidth(), parent->getHeight() / 2);
        decorateProgressItem(progressBox, gui::Alignment::Vertical::Bottom);
        auto progressBar =
            new gui::HBarGraph(progressBox, 0, 0, mrStyle::progress::boxesCount, gui::BarGraphStyle::Heavy);
        decorateProgressItem(progressBar, gui::Alignment::Vertical::Center);
        return progressBar;
    }

    gui::Text *createTimer(gui::Item *parent)
    {
        auto timer = new gui::Text(parent,
                                   0,
                                   0,
                                   style::bell_base_layout::w,
                                   style::bell_base_layout::outer_layouts_h - mrStyle::timer::timerMarginBottom);
        timer->setFont(mrStyle::timer::font);
        timer->setMargins(gui::Margins(0, mrStyle::timer::timerMarginBottom, 0, 0));
        decorateProgressItem(timer, gui::Alignment::Vertical::Top);
        return timer;
    }
} // namespace

namespace gui
{
    MeditationRunningWindow::MeditationRunningWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::meditation::MeditationProgressContract::Presenter> &&windowPresenter)
        : AppWindow(app, gui::name::window::meditationProgress), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void MeditationRunningWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);

        buildLayout();
        configureTimer();
    }

    void MeditationRunningWindow::buildLayout()
    {
        auto body = new gui::BellBaseLayout(this, 0, 0, style::bell_base_layout::w, style::bell_base_layout::h, false);
        auto vBox =
            new VBox(body->getCenterBox(), 0, 0, style::bell_base_layout::w, style::bell_base_layout::center_layout_h);

        decorateProgressItem(vBox, gui::Alignment::Vertical::Top);
        createTitle(vBox);
        progress = createProgress(vBox);
        timer    = createTimer(body->lastBox);

        time = new BellStatusClock(body->firstBox);
        time->setMaximumSize(body->firstBox->getWidth(), body->firstBox->getHeight());
        time->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        body->firstBox->resizeItems();

        dimensionChangedCallback = [&](Item &, const BoundingBox &newDim) -> bool {
            body->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }

    void MeditationRunningWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        AppWindow::onBeforeShow(mode, data);

        if (mode == ShowMode::GUI_SHOW_INIT) {
            playGong();
            presenter->start();
        }
        else {
            presenter->resume();
        }
    }

    bool MeditationRunningWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            presenter->pause();
            return true;
        }
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_RF)) {
            presenter->abandon();
            return true;
        }

        return AppWindow::onInput(inputEvent);
    }

    void MeditationRunningWindow::pregressFinished()
    {
        endSession();
    }

    void MeditationRunningWindow::intervalReached()
    {
        intervalTimeout();
    }

    void MeditationRunningWindow::configureTimer()
    {
        auto progressTimer = std::make_unique<app::ProgressTimer>(
            application, *this, meditationProgressTimerName, baseTick, meditationProgressMode);
        progressTimer->attach(progress);
        progressTimer->attach(timer);
        presenter->setTimer(std::move(progressTimer));
    }

    void MeditationRunningWindow::setTime(std::time_t newTime)
    {
        time->setTime(newTime);
        time->setTimeFormatSpinnerVisibility(true);
    }

    void MeditationRunningWindow::setTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        time->setTimeFormat(fmt);
    }

    bool MeditationRunningWindow::updateTime()
    {
        if (presenter != nullptr) {
            presenter->handleUpdateTimeEvent();
        }
        return true;
    }

    void MeditationRunningWindow::intervalTimeout()
    {
        playGong();
    }

    void MeditationRunningWindow::endSession()
    {
        playGong();
        presenter->finish();
    }

    void MeditationRunningWindow::playGong()
    {
        AudioServiceAPI::PlaybackStart(
            application, audio::PlaybackType::Meditation, purefs::dir::getCurrentOSPath() / meditationAudioPath);
    }
} // namespace gui
