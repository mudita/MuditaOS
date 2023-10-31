// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DeadBatteryInfoWindow.hpp"
#include "DeadBatteryInfoWindowData.hpp"

#include <i18n/i18n.hpp>
#include <Image.hpp>

namespace
{
    using namespace std::chrono_literals;
    constexpr auto windowTimeout = 5s;
} // namespace

namespace gui
{
    DeadBatteryInfoWindow::DeadBatteryInfoWindow(app::ApplicationCommon *app, const std::string &name)
        : WindowWithTimer(app, name, windowTimeout)
    {
        buildInterface();
        preventsAutoLock = true;
    }

    DeadBatteryInfoWindow::~DeadBatteryInfoWindow()
    {
        destroyInterface();
    }

    status_bar::Configuration DeadBatteryInfoWindow::configureStatusBar(status_bar::Configuration appConfiguration)
    {
        appConfiguration.enable(status_bar::Indicator::Time);
        appConfiguration.disable(status_bar::Indicator::NetworkAccessTechnology);
        appConfiguration.disable(status_bar::Indicator::PhoneMode);
        appConfiguration.disable(status_bar::Indicator::Signal);
        return appConfiguration;
    }

    void DeadBatteryInfoWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void DeadBatteryInfoWindow::buildInterface()
    {
        AppWindow::buildInterface();

        navBar->setActive(nav_bar::Side::Left, false);
        navBar->setActive(nav_bar::Side::Center, false);
        navBar->setActive(nav_bar::Side::Right, false);

        icon = new Icon(this,
                        style::window::default_left_margin,
                        style::window::default_vertical_pos,
                        style::window::default_body_width,
                        style::window::default_body_height,
                        "info_128px_W_G",
                        "");
        icon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        icon->image->setMargins(Margins(0, icon::image_top_margin, 0, icon::image_bottom_margin));

        setTitle(utils::translate("app_desktop_info"));
    }

    void DeadBatteryInfoWindow::destroyInterface()
    {
        erase();
    }

    void DeadBatteryInfoWindow::onBeforeShow([[maybe_unused]] gui::ShowMode mode, gui::SwitchData *data)
    {
        WindowWithTimer::onBeforeShow(mode, data);
        if (const auto switchData = dynamic_cast<DeadBatteryInfoWindowData *>(data); switchData != nullptr) {
            icon->text->setText(switchData->getMessageText());
        }
    }

    bool DeadBatteryInfoWindow::onInput([[maybe_unused]] const gui::InputEvent &inputEvent)
    {
        return true; // Ignore all inputs
    }
} // namespace gui
