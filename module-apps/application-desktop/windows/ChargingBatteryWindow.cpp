// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ChargingBatteryWindow.hpp"
#include "InputEvent.hpp"
#include "Names.hpp"

#include <gui/widgets/Image.hpp>
#include <log/log.hpp>
#include <service-appmgr/Controller.hpp>

namespace gui
{
    namespace
    {
        constexpr inline auto imgPositionX = 176;
        constexpr inline auto imgPositionY = 250;
    } // namespace

    ChargingBatteryWindow::ChargingBatteryWindow(app::ApplicationCommon *app)
        : AppWindow(app, app::window::name::dead_battery)
    {
        buildInterface();
        preventsAutoLock = true;
    }

    void ChargingBatteryWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void ChargingBatteryWindow::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setVisible(false);
        statusBar->setVisible(false);
        new gui::Image(this, imgPositionX, imgPositionY, 0, 0, "charging_battery_W_G");
    }

    void ChargingBatteryWindow::destroyInterface()
    {
        erase();
    }

    bool ChargingBatteryWindow::onInput(const InputEvent &inputEvent)
    {
        // Ignore all inputs
        return true;
    }

} /* namespace gui */
