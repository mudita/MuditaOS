// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MmiConfirmationWindow.hpp"

#include <i18n/i18n.hpp>
#include <Image.hpp>
#include <apps-common/ApplicationCommon.hpp>
#include <service-appmgr/data/MmiActionsParams.hpp>

using namespace gui;

namespace style::desktop
{
    namespace image
    {
        constexpr uint32_t x = 176;
        constexpr uint32_t y = 135;
    } // namespace image
    namespace text
    {
        constexpr uint32_t x = 40;
        constexpr uint32_t y = 300;
        constexpr uint32_t w = 400;
        constexpr uint32_t h = 300;
    } // namespace text

} // namespace style::desktop

MmiConfirmationWindow::MmiConfirmationWindow(app::ApplicationCommon *app, const std::string &name)
    : gui::WindowWithTimer(app, name)
{
    AppWindow::buildInterface();
    navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::ok));
    icon = new Icon(this,
                    style::window::default_left_margin,
                    style::window::default_vertical_pos,
                    style::window::default_body_width,
                    style::window::default_body_height,
                    "progress_128px_W_G",
                    utils::translate("app_desktop_info_mmi_confirmation"));
    icon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
    icon->image->setMargins(Margins(0, icon::image_top_margin, 0, icon::image_bottom_margin));
}

bool MmiConfirmationWindow::onInput(const InputEvent &inputEvent)
{
    if (inputEvent.isShortRelease()) {
        switch (inputEvent.getKeyCode()) {
        case KeyCode::KEY_ENTER: {
            application->returnToPreviousWindow();
            return true;
        }
        case KeyCode::KEY_RF: {
            return true;
        }
        default:
            break;
        }
    }
    return AppWindow::onInput(inputEvent);
}

void MmiConfirmationWindow::destroyInterface()
{
    erase();
}

MmiConfirmationWindow::~MmiConfirmationWindow()
{
    destroyInterface();
}
