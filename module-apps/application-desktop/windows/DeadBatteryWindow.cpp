// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DeadBatteryWindow.hpp"
#include "InputEvent.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/TopBar.hpp"
#include "log/log.hpp"
#include <application-desktop/windows/Names.hpp>
#include <service-appmgr/model/ApplicationManager.hpp>
#include <service-appmgr/Controller.hpp>
#include <Image.hpp>

namespace gui
{
    namespace
    {
        constexpr inline auto SHUTDOWN_TIMER_MS = 500;
        constexpr inline auto IMG_POS_X         = 176;
        constexpr inline auto IMG_POS_Y         = 250;
    } // namespace

    DeadBatteryWindow::DeadBatteryWindow(app::Application *app) : AppWindow(app, app::window::name::dead_battery)
    {
        buildInterface();
    }

    void DeadBatteryWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void DeadBatteryWindow::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setVisible(false);
        topBar->setVisible(false);
        new gui::Image(this, IMG_POS_X, IMG_POS_Y, 0, 0, "dead_battery_W_G");
    }

    void DeadBatteryWindow::destroyInterface()
    {
        erase();
    }
} /* namespace gui */
