// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-gui/gui/input/InputEvent.hpp>
#include <module-gui/gui/widgets/Arc.hpp>
#include "HomeModesWindow.hpp"

namespace gui
{
    HomeModesWindow::HomeModesWindow(app::Application *app, const std::string &name) : AppWindow(app, name)
    {
        buildInterface();
    }

    void HomeModesWindow::buildInterface()
    {
        AppWindow::buildInterface();
        modesBox = new ModesBox(this, style::window::modes::left_offset, style::window::modes::top_offset);
    }

    void HomeModesWindow::rebuild()
    {}

    void HomeModesWindow::destroyInterface()
    {
        erase();
    }

    HomeModesWindow::~HomeModesWindow()
    {
        destroyInterface();
    }

    void HomeModesWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {}
} // namespace gui
