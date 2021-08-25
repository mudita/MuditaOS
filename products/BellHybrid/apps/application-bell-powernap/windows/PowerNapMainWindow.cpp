// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerNapMainWindow.hpp"

namespace gui
{
    PowerNapMainWindow::PowerNapMainWindow(
        app::Application *app, std::unique_ptr<app::powernap::PowerNapMainWindowContract::Presenter> &&windowPresenter)
        : AppWindow(app, gui::name::window::main_window), windowPresenter{std::move(windowPresenter)}
    {
        this->windowPresenter->attach(this);
        buildInterface();
    }

} // namespace gui
