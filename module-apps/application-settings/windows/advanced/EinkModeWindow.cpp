// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EinkModeWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>

#include <Application.hpp>
#include <service-eink/Common.hpp>
#include <service-eink/messages/EinkModeMessage.hpp>

namespace gui
{

    EinkModeWindow::EinkModeWindow(app::ApplicationCommon *app) : AppWindow(app, window::name::eink_mode)
    {
        AppWindow::buildInterface();
        navBar->setActive(nav_bar::Side::Center, true);
        navBar->setActive(nav_bar::Side::Right, true);
        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::select));
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));

        setTitle(window::name::eink_mode);
        auto label               = new Label(this, 100, 200, 300, 50, "Change mode on click");
        label->activatedCallback = [this](Item &) -> bool {
            static auto last_mode = service::eink::EinkModeMessage::Mode::Normal;
            if (last_mode == service::eink::EinkModeMessage::Mode::Normal) {
                last_mode = service::eink::EinkModeMessage::Mode::Invert;
            }
            else {
                last_mode = service::eink::EinkModeMessage::Mode::Normal;
            }

            application->bus.sendUnicastSync(
                std::make_shared<service::eink::EinkModeMessage>(last_mode), service::name::eink, 5000);
            return true;
        };
        setFocusItem(label);
    }

} /* namespace gui */
