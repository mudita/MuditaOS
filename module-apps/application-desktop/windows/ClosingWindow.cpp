// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ClosingWindow.hpp"
#include "Names.hpp"

namespace gui
{
    ClosingWindow::ClosingWindow(app::ApplicationCommon *app) : AppWindow(app, app::window::name::closing_window)
    {
        buildInterface();
    }

    void ClosingWindow::buildInterface()
    {
        AppWindow::buildInterface();
        navBar->setVisible(false);
        statusBar->setVisible(false);
    }
} /* namespace gui */
