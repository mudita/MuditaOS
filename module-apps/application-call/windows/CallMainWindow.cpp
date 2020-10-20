// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <Label.hpp>
#include "CallMainWindow.hpp"
#include <Style.hpp>

namespace gui
{

    CallMainWindow::CallMainWindow(app::Application *app) : AppWindow(app, gui::name::window::main_window)
    {
        buildInterface();
    }

    void CallMainWindow::rebuild()
    {}
    void CallMainWindow::buildInterface()
    {
        AppWindow::buildInterface();
        setTitle(getName());
    }
    void CallMainWindow::destroyInterface()
    {
        AppWindow::destroyInterface();
    }

    CallMainWindow::~CallMainWindow()
    {}

} /* namespace gui */
