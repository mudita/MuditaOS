// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ReadyGoingWindow.hpp"
#include "MeditationRunningWindow.hpp"

#include "log.hpp"

namespace gui
{
    ReadyGoingWindow::ReadyGoingWindow(app::Application *app) : IconTextWindow(app, gui::name::window::ready_going)
    {}

    void ReadyGoingWindow::onTimeout()
    {
        LOG_DEBUG("onTimeout: timer=%ld, interval=%ld", item.getTimerSecs(), item.getIntervalSecs());
        gotoWindow(gui::name::window::meditation_running);
    }

    std::vector<std::string> ReadyGoingWindow::getText()
    {
        std::vector<std::string> content;
        content.emplace_back("Put down Bell Harmony");
        content.emplace_back("and wait for the gong");
        return content;
    }

    uint32_t ReadyGoingWindow::getTimeout()
    {
        return 5;
    }
} // namespace gui
