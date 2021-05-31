// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DeadBatteryWindow.hpp"
#include "InputEvent.hpp"
#include "gui/widgets/Image.hpp"
#include "log/log.hpp"
#include <application-desktop/windows/Names.hpp>
#include <service-appmgr/model/ApplicationManager.hpp>
#include <service-appmgr/Controller.hpp>
#include <Image.hpp>

namespace gui
{
    namespace
    {
        constexpr inline auto imgPositionX = 176;
        constexpr inline auto imgPositionY = 250;
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
        statusBar->setVisible(false);
        new gui::Image(this, imgPositionX, imgPositionY, 0, 0, "dead_battery_W_G");
    }

    void DeadBatteryWindow::destroyInterface()
    {
        erase();
    }

    bool DeadBatteryWindow::onInput(const InputEvent &inputEvent)
    {
        // Ignore all inputs
        return true;
    }
} /* namespace gui */
