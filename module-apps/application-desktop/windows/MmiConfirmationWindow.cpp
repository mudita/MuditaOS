// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MmiConfirmationWindow.hpp"
#include "Names.hpp"
#include <service-appmgr/data/MmiActionsParams.hpp>
#include <service-cellular/CellularServiceAPI.hpp>

using namespace gui;

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

MmiConfirmationWindow::MmiConfirmationWindow(app::ApplicationCommon *app, const std::string &name)
    : AppWindow(app, name)
{
    AppWindow::buildInterface();
    navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::abort));
    icon = new Icon(this,
                    style::window::default_left_margin,
                    style::window::default_vertical_pos,
                    style::window::default_body_width,
                    style::window::default_body_height,
                    "progress_128px_W_G",
                    utils::translate("app_desktop_info_mmi_confirmation"));
    icon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
    icon->image->setMargins(Margins(0, icon::image_top_margin, 0, icon::image_bottom_margin));

    preventsAutoLock = true;
}

bool MmiConfirmationWindow::onInput(const InputEvent &inputEvent)
{
    if (inputEvent.isShortRelease() && inputEvent.getKeyCode() == KeyCode::KEY_RF) {
        CellularServiceAPI::USSDRequest(application, cellular::USSDMessage::RequestType::abortSession, "");

        auto data =
            std::make_unique<mmiactions::MMIResultParams>(mmiactions::MMIResultParams::MMIResult::AbortedByUser);
        application->switchWindow(app::window::name::desktop_mmi_internal, std::move(data));

        return true;
    }

    /* Prevent locking the phone by long pound press */
    if (inputEvent.isLongRelease() && inputEvent.getKeyCode() == KeyCode::KEY_PND) {
        return true;
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
