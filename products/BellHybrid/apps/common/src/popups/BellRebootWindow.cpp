// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "popups/BellRebootWindow.hpp"
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Icon.hpp>
#include <i18n/i18n.hpp>
#include <Application.hpp>

namespace gui
{

    BellRebootWindow::BellRebootWindow(app::ApplicationCommon *app,
                                       std::unique_ptr<AbstractPowerOffPresenter> presenter,
                                       const char *name)
        : WindowWithTimer(app, name), presenter(std::move(presenter))
    {
        buildInterface();

        timerCallback = [this](Item &, sys::Timer &) {
            this->presenter->reboot();
            return true;
        };
    }
    void BellRebootWindow::buildInterface()
    {
        WindowWithTimer::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        icon = new Icon(this,
                        0,
                        0,
                        style::window_width,
                        style::window_height,
                        "bell_mudita_logo_W_G",
                        utils::translate("app_bell_reset_message"));
        icon->text->setFont(style::window::font::verybiglight);
    }
    bool BellRebootWindow::onInput(const InputEvent &)
    {
        return false;
    }
} // namespace gui
