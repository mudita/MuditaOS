// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "windows/BellTurnOffWindow.hpp"
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Icon.hpp>
#include <i18n/i18n.hpp>
#include <Application.hpp>

#include <system/Common.hpp>

namespace gui
{
    BellTurnOffWindow::BellTurnOffWindow(app::ApplicationCommon *app,
                                         std::unique_ptr<AbstractPowerOffPresenter> presenter)
        : WindowWithTimer(app, name), presenter(std::move(presenter))
    {
        buildInterface();

        timerCallback = [this](Item &, sys::Timer &) {
            this->presenter->powerOff();
            return true;
        };
    }

    void gui::BellTurnOffWindow::buildInterface()
    {
        WindowWithTimer::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        bottomBar->setVisible(false);

        icon =
            new Icon(this, 0, 0, style::window_width, style::window_height, {}, utils::translate("app_bell_goodbye"));
        icon->image->setVisible(false);
        icon->text->setFont(style::window::font::verybiglight);
    }
    bool BellTurnOffWindow::onInput(const InputEvent &)
    {
        return false;
    }

} // namespace gui
