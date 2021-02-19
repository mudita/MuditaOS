// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "LogoWindow.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"
#include "log/log.hpp"
#include <application-desktop/windows/Names.hpp>

namespace gui
{
    LogoWindow::LogoWindow(app::Application *app) : AppWindow(app, app::window::name::logo_window)
    {
        buildInterface();
    }

    void LogoWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void LogoWindow::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setVisible(false);
        topBar->setVisible(false);
        new gui::Image(this, 0, 0, 0, 0, "logo");
    }

    void LogoWindow::destroyInterface()
    {
        erase();
    }
} /* namespace gui */
