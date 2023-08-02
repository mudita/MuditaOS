﻿// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Mmi.hpp"
#include "MmiPushWindow.hpp"
#include "Names.hpp"

#include <i18n/i18n.hpp>
#include <Image.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <service-appmgr/data/MmiActionsParams.hpp>

using namespace gui;

// move to style
namespace style::desktop
{
    namespace image
    {
        constexpr std::uint32_t x = 176;
        constexpr std::uint32_t y = 135;
    } // namespace image

    namespace text
    {
        constexpr std::uint32_t x = 40;
        constexpr std::uint32_t y = 300;
        constexpr std::uint32_t w = 400;
        constexpr std::uint32_t h = 300;
    } // namespace text
} // namespace style::desktop

MmiPushWindow::MmiPushWindow(app::ApplicationCommon *app, const std::string &name) : gui::AppWindow(app, name)
{
    AppWindow::buildInterface();
    navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::ok));
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

void MmiPushWindow::onBeforeShow([[maybe_unused]] ShowMode mode, SwitchData *data)
{
    if (const auto metadata = dynamic_cast<app::manager::actions::MMIParams *>(data); metadata != nullptr) {
        icon->text->setText(mmi::removePhrase(metadata->getData(), "\r"));
    }
}

bool MmiPushWindow::onInput(const InputEvent &inputEvent)
{
    if (inputEvent.isShortRelease()) {
        switch (inputEvent.getKeyCode()) {
        case KeyCode::KEY_ENTER: {
            application->switchWindow(app::window::name::desktop_main_window);
            return true;
        }

        case KeyCode::KEY_RF:
            return true;

        default:
            break;
        }
    }

    /* Prevent leaving the window by long RF press */
    if (inputEvent.isLongRelease() && inputEvent.getKeyCode() == KeyCode::KEY_RF) {
        return true;
    }

    return AppWindow::onInput(inputEvent);
}

void MmiPushWindow::destroyInterface()
{
    erase();
}

MmiPushWindow::~MmiPushWindow()
{
    destroyInterface();
}
