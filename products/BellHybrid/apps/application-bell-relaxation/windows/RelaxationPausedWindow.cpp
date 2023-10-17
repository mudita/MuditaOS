// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationPausedWindow.hpp"
#include <data/RelaxationStyle.hpp>
#include <ApplicationBellRelaxation.hpp>

#include <apps-common/widgets/BellBaseLayout.hpp>
#include <i18n/i18n.hpp>

namespace
{
    gui::BellStatusClock *createClock(gui::Item *parent)
    {
        auto time = new gui::BellStatusClock(parent);
        time->setFont(gui::relaxationStyle::clockFont);
        time->setMaximumSize(parent->getWidth(), parent->getHeight());
        time->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        return time;
    }
} // namespace

namespace gui
{
    RelaxationPausedWindow::RelaxationPausedWindow(
        app::ApplicationCommon *app, std::unique_ptr<app::relaxation::RelaxationPausedContract::Presenter> &&presenter)
        : AppWindow(app, gui::window::name::relaxationPaused), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void RelaxationPausedWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        presenter->onBeforeShow();
        updateTime();
    }

    void RelaxationPausedWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        auto body = new gui::BellBaseLayout(this, 0, 0, style::bell_base_layout::w, style::bell_base_layout::h, false);
        auto vBox =
            new VBox(body->getCenterBox(), 0, 0, style::bell_base_layout::w, style::bell_base_layout::center_layout_h);
        vBox->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        vBox->setEdges(gui::RectangleEdge::None);

        new gui::Image(vBox, "big_pause_W_G");

        time = createClock(body->firstBox);

        vBox->resizeItems();
        body->resizeItems();
    }

    bool RelaxationPausedWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            application->returnToPreviousWindow();
            return true;
        }

        if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            application->switchWindow(gui::name::window::main_window);
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

    void RelaxationPausedWindow::setTime(std::time_t newTime)
    {
        time->setTime(newTime);
        time->setTimeFormatSpinnerVisibility(true);
    }

    void RelaxationPausedWindow::setTimeFormat(utils::time::Locale::TimeFormat fmt)
    {
        time->setTimeFormat(fmt);
    }

    RefreshModes RelaxationPausedWindow::updateTime()
    {
        if (presenter) {
            presenter->handleUpdateTimeEvent();
        }
        return RefreshModes::GUI_REFRESH_FAST;
    }
} // namespace gui
