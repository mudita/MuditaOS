// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerNapSessionEndedWindow.hpp"
#include "data/PowerNapStyle.hpp"
#include <application-bell-powernap/ApplicationBellPowerNap.hpp>
#include <RichTextParser.hpp>
#include <BoxLayout.hpp>
#include <Image.hpp>
#include <Text.hpp>
#include <gui/core/FontManager.hpp>

namespace gui
{
    PowerNapSessionEndedWindow::PowerNapSessionEndedWindow(
        app::Application *app, std::shared_ptr<app::powernap::PowerNapSessionEndedContract::Presenter> presenter)
        : WindowWithTimer(app, gui::window::name::powernapSessionEnded), presenter{std::move(presenter)}
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
        auto body = new gui::VBox(this, 0, 0, style::window_width, style::window_height);
        body->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        new gui::Image(body, "tethering_notification_icon"); // TODO: ICON!!!!

        auto text = new gui::Text(body, 0, 0, body->getWidth(), powerNapStyle::sessionEnd::textH);
        TextFormat format(FontManager::getInstance().getFont(powerNapStyle::descriptionFont));
        gui::text::RichTextParser parser;
        auto textParsed = parser.parse(utils::translate("app_bell_powernap_session_ended_message"), &format);
        text->setText(std::move(textParsed));
        text->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        text->setVisible(true);
    }

    void PowerNapSessionEndedWindow::registerCallbacks()
    {
        timerCallback = [this](Item &, sys::Timer &timer) {
            presenter->activate();
            return true;
        };
    }
} // namespace gui
