// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsProgressWindow.hpp"
#include <ApplicationBellBackgroundSounds.hpp>
#include "data/BGSoundsStyle.hpp"
#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/widgets/BarGraph.hpp>
#include <apps-common/widgets/ProgressTimerWithBarGraphAndCounter.hpp>
#include <apps-common/GuiTimer.hpp>
#include <TextFixedSize.hpp>
#include <time/dateCommon.hpp>
namespace
{
    inline constexpr auto bgSoundsTimerName = "BGSoundsProgressTimer";
    inline constexpr std::chrono::seconds timerTick{1};

    void decorateProgressItem(gui::Rect *item, gui::Alignment::Vertical alignment)
    {
        item->setEdges(gui::RectangleEdge::None);
        item->activeItem = false;
        item->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, alignment));
    }
    gui::Text *createTitle(gui::VBox *parent)
    {
        namespace bgStyle = gui::bgSoundsStyle;
        auto title        = new gui::TextFixedSize(parent, 0, 0, 0, 0);
        title->setFont(bgStyle::titleFont);
        title->setMinimumWidth(bgStyle::progress::titleWidth);
        title->setMinimumHeightToFitText(bgStyle::progress::titleMaxLines);
        title->drawUnderline(false);

        decorateProgressItem(title, gui::Alignment::Vertical::Top);
        return title;
    }
    gui::HBarGraph *createProgress(gui::VBox *parent)
    {
        auto progressBox = new gui::HBox(parent, 0, 0, 0, 0);
        decorateProgressItem(progressBox, gui::Alignment::Vertical::Bottom);
        progressBox->setMaximumSize(parent->getWidth(), parent->getHeight() / 2);
        auto progressBar = new gui::HBarGraph(
            progressBox, 0, 0, gui::bgSoundsStyle::progress::maxProgressValue, gui::BarGraphStyle::Heavy);
        decorateProgressItem(progressBar, gui::Alignment::Vertical::Center);
        return progressBar;
    }
    gui::TimeFixedWidget *createTimer(gui::Item *parent)
    {
        using namespace style;
        using namespace gui::bgSoundsStyle;

        auto timer = new gui::TimeFixedWidget(parent,
                                              0,
                                              0,
                                              bell_base_layout::outer_layouts_w,
                                              bell_base_layout::outer_layouts_h - progress::bottomDescTopMargin);
        timer->setFont(bottomTimerFont);
        timer->setMargins(gui::Margins(0, progress::bottomDescTopMargin, 0, 0));
        decorateProgressItem(timer, gui::Alignment::Vertical::Top);
        return timer;
    }
    gui::BellStatusClock *createClock(gui::Item *parent)
    {
        auto time = new gui::BellStatusClock(parent);
        time->setMaximumSize(parent->getWidth(), parent->getHeight());
        time->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
        return time;
    }
} // namespace

namespace gui
{
    BGSoundsProgressWindow::BGSoundsProgressWindow(
        app::ApplicationCommon *app, std::shared_ptr<app::bgSounds::BGSoundsProgressContract::Presenter> presenter)
        : AppWindow(app, gui::window::name::bgSoundsProgress), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void BGSoundsProgressWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        presenter->onBeforeShow();
        updateTime();

        if (mode == ShowMode::GUI_SHOW_RETURN && presenter->isPaused()) {
            presenter->resume();
            return;
        }

        if (data && typeid(*data) == typeid(BGSoundsSwitchData)) {
            auto *audioSwitchData = static_cast<BGSoundsSwitchData *>(data);
            audioContext          = audioSwitchData->getAudioContext();
            title->setText(audioContext->getSound().tags.title);
            presenter->activate(audioContext->getSound());
        }
    }

    void BGSoundsProgressWindow::buildInterface()
    {
        AppWindow::buildInterface();
        buildLayout();
        configureTimer();
    }
    void BGSoundsProgressWindow::buildLayout()
    {
        statusBar->setVisible(false);
        auto body = new gui::BellBaseLayout(this, 0, 0, style::bell_base_layout::w, style::bell_base_layout::h, false);
        auto vBox =
            new VBox(body->getCenterBox(), 0, 0, style::bell_base_layout::w, style::bell_base_layout::center_layout_h);
        decorateProgressItem(vBox, gui::Alignment::Vertical::Top);
        title       = createTitle(vBox);
        progressBar = createProgress(vBox);
        timerText   = createTimer(body->lastBox);
        time        = createClock(body->firstBox);
        body->firstBox->resizeItems();

        body->lastBox->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        body->lastBox->resizeItems();
        vBox->resizeItems();

        dimensionChangedCallback = [&](Item &, const BoundingBox &newDim) -> bool {
            body->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }
    void BGSoundsProgressWindow::configureTimer()
    {
        auto timer = std::make_unique<app::ProgressTimerWithBarGraphAndCounter>(
            application, *this, bgSoundsTimerName, timerTick, app::ProgressCountdownMode::Increasing);
        timer->attach(progressBar);
        timer->attach(timerText);
        presenter->setTimer(std::move(timer));
    }

    auto BGSoundsProgressWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        if (inputEvent.isShortRelease()) {
            if (inputEvent.is(KeyCode::KEY_RF)) {
                presenter->stop();
                return true;
            }
            else if (inputEvent.is(KeyCode::KEY_ENTER)) {
                presenter->pause();
                return true;
            }
            else if (inputEvent.is(KeyCode::KEY_DOWN) || inputEvent.is(KeyCode::KEY_UP)) {
                application->switchWindow(gui::popup::window::volume_window);
                return true;
            }
        }
        return AppWindow::onInput(inputEvent);
    }
    void BGSoundsProgressWindow::onFinished()
    {
        application->returnToPreviousWindow();
    }
    void BGSoundsProgressWindow::onPaused()
    {
        application->switchWindow(gui::window::name::bgSoundsPaused);
    }
    void BGSoundsProgressWindow::setTime(std::time_t newTime)
    {
        time->setTime(newTime);
        time->setTimeFormatSpinnerVisibility(true);
    }

    void BGSoundsProgressWindow::setTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        time->setTimeFormat(fmt);
    }

    RefreshModes BGSoundsProgressWindow::updateTime()
    {
        if (presenter) {
            presenter->handleUpdateTimeEvent();
        }
        return RefreshModes::GUI_REFRESH_NONE;
    }
} // namespace gui
