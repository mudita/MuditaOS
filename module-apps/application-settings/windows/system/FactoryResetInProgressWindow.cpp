// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FactoryResetInProgressWindow.hpp"
#include <application-settings/windows/WindowNames.hpp>

#include <i18n/i18n.hpp>

namespace gui
{
    FactoryResetInProgressWindow::FactoryResetInProgressWindow(app::ApplicationCommon *app)
        : AppWindow(app, gui::window::name::factory_reset_in_progress)
    {
        buildInterface();
    }

    void FactoryResetInProgressWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        navBar->setVisible(false);

        infoIcon = new gui::Icon(this,
                                 style::window::default_left_margin,
                                 style::window::default_vertical_pos,
                                 style::window::default_body_width,
                                 style::window::default_body_height,
                                 "",
                                 utils::translate(""));
        infoIcon->setAlignment(Alignment::Horizontal::Center);
        infoIcon->image->set("progress_128px_W_G");
        infoIcon->text->setText(utils::translate("app_settings_factory_reset_in_progress"));
    }

} /* namespace gui */
