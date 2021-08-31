// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "StatusBarImageTypeWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>
#include <Application.hpp>
#include <TextFixedSize.hpp>
#include <status-bar/Style.hpp>

namespace gui
{
    StatusBarImageTypeWindow::StatusBarImageTypeWindow(app::Application *app)
        : AppWindow(app, window::name::status_bar_img_type)
    {
        AppWindow::buildInterface();
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        setTitle(window::name::status_bar_img_type);

#if DEVELOPER_SETTINGS_OPTIONS == 1
        auto body = new VBox(this,
                             style::window::default_left_margin,
                             style::window::default_vertical_pos,
                             style::window::default_body_width,
                             style::window::default_body_height);
        body->setEdges(RectangleEdge::None);
        body->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        auto selectedMode = new TextFixedSize(body, 0, 0, 0, 0);
        selectedMode->setMaximumWidth(style::window::default_body_width);
        selectedMode->drawUnderline(false);
        selectedMode->setMinimumHeightToFitText(1);
        selectedMode->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        selectedMode->setEditMode(EditMode::Browse);
        selectedMode->setPenFocusWidth(2);
        selectedMode->setEdges(RectangleEdge::All);

        auto selectedString =
            std::string("Currently selected image type: [" +
                        std::string(magic_enum::enum_name(style::status_bar::imageTypeSpecifier)) + "]");
        selectedMode->setText(selectedString);

        selectedMode->activatedCallback = [this, body, selectedMode](Item &) -> bool {
            if (style::status_bar::imageTypeSpecifier == ImageTypeSpecifier::W_M) {
                style::status_bar::imageTypeSpecifier = ImageTypeSpecifier::W_G;
            }
            else {
                style::status_bar::imageTypeSpecifier = ImageTypeSpecifier::W_M;
            }

            auto selectedString =
                std::string("Currently selected image type: [" +
                            std::string(magic_enum::enum_name(style::status_bar::imageTypeSpecifier)) + "]");
            selectedMode->setText(selectedString);

            body->resizeItems();
            application->refreshWindow(RefreshModes::GUI_REFRESH_DEEP);
            return true;
        };

        setFocusItem(selectedMode);
        body->resizeItems();
#endif
    }

    status_bar::Configuration StatusBarImageTypeWindow::configureStatusBar(status_bar::Configuration appConfiguration)
    {
        appConfiguration.disable(status_bar::Indicator::Time);
        appConfiguration.enable(status_bar::Indicator::Lock);
        appConfiguration.enable(status_bar::Indicator::PhoneMode);
        appConfiguration.enable(status_bar::Indicator::Battery);
        appConfiguration.enable(status_bar::Indicator::Signal);
        appConfiguration.enable(status_bar::Indicator::SimCard);
        appConfiguration.enable(status_bar::Indicator::Bluetooth);
        return appConfiguration;
    }
} /* namespace gui */
