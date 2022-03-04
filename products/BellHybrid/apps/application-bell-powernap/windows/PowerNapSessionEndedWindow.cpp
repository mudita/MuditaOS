// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerNapSessionEndedWindow.hpp"
#include "data/PowerNapStyle.hpp"
#include <application-bell-powernap/ApplicationBellPowerNap.hpp>
#include <gui/widgets/Icon.hpp>

namespace gui
{
    PowerNapSessionEndedWindow::PowerNapSessionEndedWindow(
        app::ApplicationCommon *app, std::shared_ptr<app::powernap::PowerNapSessionEndedContract::Presenter> presenter)
        : WindowWithTimer(app, window::name::powernapSessionEnded), presenter{std::move(presenter)}
    {
        buildInterface();
    }

    void PowerNapSessionEndedWindow::buildInterface()
    {
        AppWindow::buildInterface();
        buildLayout();
        registerCallbacks();
    }

    void PowerNapSessionEndedWindow::buildLayout()
    {
        statusBar->setVisible(false);
        auto body = new VBox(this, 0, 0, style::window_width, style::window_height);
        body->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        auto icon =
            new Icon(this, 0, 0, style::window_width, style::window_height, "big_namaste", "", ImageTypeSpecifier::W_G);
        icon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        icon->image->setMargins({0, image_top_margin, 0, image_bottom_margin});
        icon->text->setFont(style::window::font::verybiglight);
    }

    void PowerNapSessionEndedWindow::registerCallbacks()
    {
        timerCallback = [this](Item &, sys::Timer &timer) {
            presenter->activate();
            return true;
        };
    }
} // namespace gui
