// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EinkModeWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>

#include <apps-common/Application.hpp>
#include <service-eink/Common.hpp>
#include <service-eink/messages/EinkModeMessage.hpp>

namespace gui
{

    EinkModeWindow::EinkModeWindow(app::Application *app) : AppWindow(app, window::name::eink_mode)
    {
        AppWindow::buildInterface();
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

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
