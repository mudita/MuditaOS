/*
 * @file CalendarMainWindow.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 3 cze 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include "gui/widgets/Window.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Item.hpp"
#include "gui/widgets/BoxLayout.hpp"
#include "CalendarMainWindow.hpp"

namespace app
{

    CalendarMainWindow::CalendarMainWindow(app::Application *app, std::string name) : AppWindow(app, name)
    {
        buildInterface();
    }

    void CalendarMainWindow::rebuild()
    {}
    void CalendarMainWindow::buildInterface()
    {
        AppWindow::buildInterface();
    }
    void CalendarMainWindow::destroyInterface()
    {
        AppWindow::destroyInterface();
    }

    CalendarMainWindow::~CalendarMainWindow()
    {
        destroyInterface();
    }

} /* namespace app */
