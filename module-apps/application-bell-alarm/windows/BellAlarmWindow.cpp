// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellAlarmWindow.hpp"

#include <module-gui/gui/input/InputEvent.hpp>

namespace gui
{
    BellAlarmWindow::BellAlarmWindow(app::Application *app, std::string name) : AppWindow(app, std::move(name))
    {
        buildInterface();
    }

    void BellAlarmWindow::buildInterface()
    {
        AppWindow::buildInterface();
        statusBar->setVisible(false);
        header->setTitleVisibility(true);
        bottomBar->setVisible(false);
    }

    bool BellAlarmWindow::onInput(const InputEvent &inputEvent)
    {
        return AppWindow::onInput(inputEvent);
    }

    void BellAlarmWindow::rebuild()
    {
        erase();
        buildInterface();
    }
} /* namespace gui */
