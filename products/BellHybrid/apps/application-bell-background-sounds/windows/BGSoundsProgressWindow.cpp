// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BGSoundsProgressWindow.hpp"
#include <ApplicationBellBackgroundSounds.hpp>
#include "data/BGSoundsStyle.hpp"
#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/widgets/BarGraph.hpp>
#include <apps-common/widgets/ProgressTimer.hpp>
#include <apps-common/GuiTimer.hpp>
#include <Text.hpp>

namespace
{
    inline constexpr auto bgSoundsTimerName     = "BGSoundsProgressTimer";
    inline constexpr std::chrono::seconds timerTick{1};

    void decorateProgressItem(gui::Rect *item, gui::Alignment::Vertical alignment)
    {
        item->setEdges(gui::RectangleEdge::None);
        item->activeItem = false;
        item->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, alignment));
    }
    gui::Text *createTitle(gui::VBox *parent)
    {
        auto title = new gui::Text(parent, 0, 0, parent->getWidth(), parent->getHeight() / 2);
        title->setFont(gui::bgSoundsStyle::descriptionFont);

        decorateProgressItem(title, gui::Alignment::Vertical::Top);
        return title;
    }
    gui::HBarGraph *createProgress(gui::VBox *parent)
    {
        auto progressBox = new gui::HBox(parent, 0, 0, parent->getWidth(), parent->getHeight() / 2);
        decorateProgressItem(progressBox, gui::Alignment::Vertical::Bottom);
        auto progressBar =
            new gui::HBarGraph(progressBox, 0, 0, gui::bgSoundsStyle::progress::boxesCount, gui::BarGraphStyle::Heavy);
        decorateProgressItem(progressBar, gui::Alignment::Vertical::Center);
        return progressBar;
    }
    gui::Text *createTimer(gui::Item *parent)
    {
        using namespace style;
        using namespace gui::bgSoundsStyle;
        auto timer = new gui::Text(
            parent, 0, 0, bell_base_layout::w, bell_base_layout::outer_layouts_h - progress::bottomDescTopMargin);
        timer->setFont(descriptionFont);
        timer->setMargins(gui::Margins(0, progress::bottomDescTopMargin, 0, 0));
        decorateProgressItem(timer, gui::Alignment::Vertical::Top);
        return timer;
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
        if (mode == ShowMode::GUI_SHOW_RETURN) {
            presenter->resume();
            return;
        }

        if (data && typeid(*data) == typeid(BGSoundsSwitchData)) {
            auto *audioSwitchData = static_cast<BGSoundsSwitchData *>(data);
            audioContext          = audioSwitchData->getAudioContext();
            title->setText(audioContext->getTags().title);
            presenter->activate(audioContext->getTags());
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
        auto body = new gui::BellBaseLayout(this, 0, 0, style::bell_base_layout::w, style::bell_base_layout::h, false);
        auto vBox =
            new VBox(body->getCenterBox(), 0, 0, style::bell_base_layout::w, style::bell_base_layout::center_layout_h);
        decorateProgressItem(vBox, gui::Alignment::Vertical::Top);
        title       = createTitle(vBox);
        progressBar = createProgress(vBox);
        timerText   = createTimer(body->lastBox);

        dimensionChangedCallback = [&](Item &, const BoundingBox &newDim) -> bool {
            body->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }
    void BGSoundsProgressWindow::configureTimer()
    {
        auto timer = std::make_unique<app::ProgressTimer>(
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
} // namespace gui
