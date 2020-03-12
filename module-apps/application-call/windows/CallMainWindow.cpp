/*
 * @file CallMainWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 lip 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */

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
