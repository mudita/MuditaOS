// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BaseSettingsWindow.hpp"

#include <i18/i18.hpp>

namespace gui
{

    BaseSettingsWindow::BaseSettingsWindow(app::Application *app, std::string name) : AppWindow(app, name)
    {
        buildInterface();
    }

    void BaseSettingsWindow::buildInterface()
    {
        AppWindow::buildInterface();

        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        topBar->setActive(TopBar::Elements::TIME, true);
    }

    void BaseSettingsWindow::destroyInterface()
    {
        erase();
        invalidate();
    }

    void BaseSettingsWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

} // namespace gui
