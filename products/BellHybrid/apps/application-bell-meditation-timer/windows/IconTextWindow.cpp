// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "IconTextWindow.hpp"

#include <i18n/i18n.hpp>

namespace gui
{
    IconTextWindow::IconTextWindow(
        app::ApplicationCommon *app,
        std::string name,
        std::unique_ptr<app::meditation::MeditationBaseContract::Presenter> &&windowPresenter)
        : WithTimerWindow(app, std::move(name)), presenter{std::move(windowPresenter)}
    {
        buildInterface();
        disableInput();
        presenter->attach(this);
    }

    void IconTextWindow::buildInterface()
    {
        WithTimerWindow::buildInterface();

        icon = new Icon(this, 0, 0, style::window_width, style::window_height, {}, {});
        icon->text->setFont(itStyle::text::font);
    }

    void IconTextWindow::destroyInterface()
    {
        erase();
        icon = nullptr;
    }

    void IconTextWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        WithTimerWindow::onBeforeShow(mode, data);

        if (mode == ShowMode::GUI_SHOW_INIT) {
            std::chrono::seconds secs = getTimeout();
            if (secs > std::chrono::seconds::zero()) {
                startTimer(secs);
            }
        }
    }

    void IconTextWindow::updateDisplay()
    {
        icon->image->set(getImageName());
        icon->text->setRichText(getText());
    }

    void IconTextWindow::buildMeditationItem(MeditationItem &item)
    {
        presenter->get(item);
    }

    void IconTextWindow::onMeditationItemAvailable(MeditationItem *item)
    {
        if (item != nullptr) {
            presenter->set(*item);
        }
    }
} // namespace gui
