// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationEndedWindow.hpp"
#include <data/RelaxationStyle.hpp>

#include <ApplicationBellRelaxation.hpp>
#include <gui/widgets/Icon.hpp>

namespace gui
{
    RelaxationEndedWindow::RelaxationEndedWindow(
        app::ApplicationCommon *app, std::unique_ptr<app::relaxation::RelaxationEndedContract::Presenter> &&presenter)
        : WindowWithTimer(app, window::name::relaxationEnded), presenter{std::move(presenter)}
    {
        buildInterface();
    }

    void RelaxationEndedWindow::buildInterface()
    {
        AppWindow::buildInterface();
        buildLayout();
        registerCallbacks();
    }

    void RelaxationEndedWindow::buildLayout()
    {
        statusBar->setVisible(false);
        auto body = new VBox(this, 0, 0, style::window_width, style::window_height);
        body->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        auto icon =
            new Icon(this, 0, 0, style::window_width, style::window_height, "big_namaste", "", ImageTypeSpecifier::W_G);
        icon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        icon->image->setMargins(
            {0, gui::relaxationStyle::ended::image_top_margin, 0, gui::relaxationStyle::ended::image_bottom_margin});
        icon->text->setFont(style::window::font::verybiglight);
    }

    void RelaxationEndedWindow::registerCallbacks()
    {
        timerCallback = [this](Item &, sys::Timer &timer) {
            presenter->activate();
            return true;
        };
    }
} // namespace gui
