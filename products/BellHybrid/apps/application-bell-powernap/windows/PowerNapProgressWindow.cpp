// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerNapProgressWindow.hpp"
#include "application-bell-powernap/ApplicationBellPowerNap.hpp"
#include "data/PowerNapStyle.hpp"
#include "data/PowerNapSwitchData.hpp"
#include <apps-common/widgets/BellBaseLayout.hpp>
#include <apps-common/widgets/BarGraph.hpp>
#include <apps-common/widgets/ProgressTimerWithBarGraphAndCounter.hpp>
#include <apps-common/GuiTimer.hpp>
#include <Text.hpp>
#include <keymap/KeyMap.hpp>

namespace
{
    void decorateProgressItem(gui::Rect *item, gui::Alignment::Vertical alignment)
    {
        item->setEdges(gui::RectangleEdge::None);
        item->activeItem = false;
        item->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, alignment));
    }
    void createTitle(gui::VBox *parent)
    {
        auto title = new gui::Text(parent, 0, 0, parent->getWidth(), parent->getHeight() / 2);
        title->setFont(gui::powerNapStyle::descriptionFont);
        title->setText(utils::translate("app_bellmain_power_nap"));
        decorateProgressItem(title, gui::Alignment::Vertical::Top);
    }
    gui::HBarGraph *createProgress(gui::VBox *parent)
    {
        auto progressBox = new gui::HBox(parent, 0, 0, parent->getWidth(), parent->getHeight() / 2);
        decorateProgressItem(progressBox, gui::Alignment::Vertical::Bottom);
        auto progressBar =
            new gui::HBarGraph(progressBox, 0, 0, gui::powerNapStyle::progress::boxesCount, gui::BarGraphStyle::Heavy);
        decorateProgressItem(progressBar, gui::Alignment::Vertical::Center);
        return progressBar;
    }
    gui::Text *createTimer(gui::Item *parent)
    {
        using namespace style;
        using namespace gui::powerNapStyle;
        auto timer = new gui::Text(
            parent, 0, 0, bell_base_layout::w, bell_base_layout::outer_layouts_h - progress::bottomDescTopMargin);
        timer->setFont(napTimerFont);
        timer->setMargins(gui::Margins(0, progress::bottomDescTopMargin, 0, 0));
        decorateProgressItem(timer, gui::Alignment::Vertical::Top);
        return timer;
    }

    inline constexpr auto powernapTimerName = "PowerNapTimer";
    inline constexpr std::chrono::seconds timerTick{1};
} // namespace

namespace gui
{
    PowerNapProgressWindow::PowerNapProgressWindow(
        app::ApplicationCommon *app, std::shared_ptr<app::powernap::PowerNapProgressContract::Presenter> presenter)
        : AppWindow(app, gui::window::name::powernapProgress), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void PowerNapProgressWindow::buildInterface()
    {
        AppWindow::buildInterface();

        buildLayout();
        configureTimer();
        // presenter->activate();
    }
    void PowerNapProgressWindow::buildLayout()
    {
        statusBar->setVisible(false);
        auto body = new gui::BellBaseLayout(this, 0, 0, style::bell_base_layout::w, style::bell_base_layout::h, false);
        auto vBox =
            new VBox(body->getCenterBox(), 0, 0, style::bell_base_layout::w, style::bell_base_layout::center_layout_h);
        decorateProgressItem(vBox, gui::Alignment::Vertical::Top);
        createTitle(vBox);
        progressBar = createProgress(vBox);
        timerText   = createTimer(body->lastBox);

        time = new BellStatusClock(body->firstBox);
        time->setMaximumSize(body->firstBox->getWidth(), body->firstBox->getHeight());
        time->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        body->firstBox->resizeItems();

        dimensionChangedCallback = [&](Item &, const BoundingBox &newDim) -> bool {
            body->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }
    void PowerNapProgressWindow::configureTimer()
    {
        auto timer = std::make_unique<app::ProgressTimerWithBarGraphAndCounter>(
            application, *this, powernapTimerName, timerTick, app::ProgressCountdownMode::Increasing);
        timer->attach(progressBar);
        timer->attach(timerText);
        presenter->setTimer(std::move(timer));
    }

    auto PowerNapProgressWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        if (inputEvent.isShortRelease()) {
            const auto key = mapKey(inputEvent.getKeyCode());
            if (presenter->isNapFinished() && key == KeyMap::LightPress) {
                presenter->endNap();
                return true;
            }
            else if (not presenter->isNapFinished() && key == KeyMap::LightPress) {
                presenter->pause();
                return true;
            }
            else if (not presenter->isNapFinished() && key == KeyMap::Back) {
                reinterpret_cast<app::Application *>(application)->resumeIdleTimer();
                presenter->endNap();
                application->returnToPreviousWindow();
                return true;
            }
            return false;
        }
        return AppWindow::onInput(inputEvent);
    }
    void PowerNapProgressWindow::napEnded()
    {
        application->switchWindow(gui::window::name::powernapSessionEnded, std::make_unique<gui::PowerNapSwitchData>());
    }

    void PowerNapProgressWindow::setTime(std::time_t newTime)
    {
        time->setTime(newTime);
        time->setTimeFormatSpinnerVisibility(true);
    }

    void PowerNapProgressWindow::setTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        time->setTimeFormat(fmt);
    }

    bool PowerNapProgressWindow::updateTime()
    {
        if (presenter) {
            presenter->handleUpdateTimeEvent();
        }
        return true;
    }

    void PowerNapProgressWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        presenter->onBeforeShow();
        updateTime();

        if (mode == ShowMode::GUI_SHOW_INIT) {
            presenter->activate();
        }
        else {
            presenter->resume();
        }
    }
} // namespace gui
