// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SessionEndWindow.hpp"

#include "log.hpp"

namespace gui
{
    SessionEndWindow::SessionEndWindow(app::Application *app) : IconTextWindow(app, gui::name::window::session_end)
    {}

    void SessionEndWindow::onTimeout()
    {
        application->popToWindow(gui::name::window::main_window);
        application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
    }

    std::vector<std::string> SessionEndWindow::getText()
    {
        std::vector<std::string> content;
        content.emplace_back("Thank you for");
        content.emplace_back("the session");
        return content;
    }

    uint32_t SessionEndWindow::getTimeout()
    {
        return 3;
    }
} // namespace gui
