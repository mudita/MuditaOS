// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "windows/BellBatteryShutdownWindow.hpp"
#include <gui/input/InputEvent.hpp>
#include <gui/widgets/Icon.hpp>
#include <i18n/i18n.hpp>
#include <Application.hpp>

namespace gui
{
    BellBatteryShutdownWindow::BellBatteryShutdownWindow(app::ApplicationCommon *app, const std::string &name)
        : gui::AppWindow(app, name)
    {
        buildInterface();
    }

    void gui::BellBatteryShutdownWindow::buildInterface()
    {
        AppWindow::buildInterface();
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        auto icon =
            new Icon(this, 0, 0, style::window_width, style::window_height, "bell_battery_status_empty_W_M", {});
        icon->text->setRichText(utils::translate("app_bell_welcome_charge_message"));
        icon->text->setFont(style::window::font::largelight);
        icon->text->setVisible(true);
        icon->resizeItems();
    }
    bool BellBatteryShutdownWindow::onInput(const InputEvent &)
    {
        return false;
    }

} // namespace gui
