// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * TestMessageWindow.cpp
 *
 *  Created on: 3 gru 2019
 *      Author: kuba
 */

#include <memory>
#include <functional>

#include "module-services/service-appmgr/model/ApplicationManager.hpp"

#include "../ApplicationSettings.hpp"

#include <i18n/i18n.hpp>

#include "Label.hpp"
#include "Margins.hpp"
#include "TestMessageWindow.hpp"
#include <Style.hpp>

#include "service-cellular/api/CellularServiceAPI.hpp"

namespace gui
{

    TestMessageWindow::TestMessageWindow(app::Application *app) : AppWindow(app, "Message")
    {
        buildInterface();
    }

    void TestMessageWindow::rebuild()
    {

        destroyInterface();
        buildInterface();
    }

    void textSetInput(Text *text, bool numeric = true)
    {
        /// tu ustawiasz że coś jest edytowalne
        text->setEditMode(EditMode::EDIT);
        text->setFont(style::window::font::bigbold);
        /// tu ustawiasz tak naprawdę przez tą listę InputMode::digit etc jakie kolejne tryby wprowadzania są wybierane
        if (numeric) {
            text->setInputMode(new InputMode({InputMode::digit}));
        }
        else {
            text->setInputMode(new InputMode({InputMode::ABC, InputMode::abc, InputMode::digit}));
        }
    }

    void TestMessageWindow::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        receivedLabel = new gui::Label(this, 10, 50, 480 - 20, 50, "Received SMS");
        receivedLabel->setAlignment(
            gui::Alignment(gui::Alignment::Vertical::Center, gui::Alignment::Horizontal::Center));
        receivedLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        receiveNumber = new gui::Label(this, 110, 190, 480 - 120, 50);
        receiveNumber->setFont(style::window::font::small);

        RecNumberLabel = new gui::Label(this, 10, 190, 90, 50, "Number");
        RecNumberLabel->setAlignment(
            gui::Alignment(gui::Alignment::Vertical::Center, gui::Alignment::Horizontal::Center));
        RecNumberLabel->setFont(style::window::font::small);
        RecNumberLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        receiveMessage = new gui::Text(this, 110, 120, 480 - 120, 50);
        textSetInput(receiveMessage);
        receiveMessage->setFont(style::window::font::small);

        RecMessageLabel = new gui::Label(this, 10, 120, 90, 50, "Message");
        RecMessageLabel->setAlignment(
            gui::Alignment(gui::Alignment::Vertical::Center, gui::Alignment::Horizontal::Center));
        RecMessageLabel->setFont(style::window::font::small);
        RecMessageLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        sendLabel = new gui::Label(this, 10, 290, 480 - 20, 50, "New SMS");
        sendLabel->setAlignment(gui::Alignment(gui::Alignment::Vertical::Center, gui::Alignment::Horizontal::Center));
        sendLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        sendMessage = new gui::Text(this, 110, 360, 480 - 120, 50);
        textSetInput(sendMessage, false);
        sendMessage->setText("Siema tu PurePhone :)");
        sendMessage->setFont(style::window::font::small);

        sendNumber = new gui::Text(this, 110, 430, 480 - 120, 50);
        textSetInput(sendNumber);
        sendNumber->setFont(style::window::font::small);

        SendNumberLabel = new gui::Label(this, 10, 430, 90, 50, "Number");
        SendNumberLabel->setAlignment(
            gui::Alignment(gui::Alignment::Vertical::Center, gui::Alignment::Horizontal::Center));
        SendNumberLabel->setFont(style::window::font::small);
        SendNumberLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        SendMessageLabel = new gui::Label(this, 10, 360, 90, 50, "Message");
        SendMessageLabel->setAlignment(
            gui::Alignment(gui::Alignment::Vertical::Center, gui::Alignment::Horizontal::Center));
        SendMessageLabel->setFont(style::window::font::small);
        SendMessageLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        sendButton = new gui::Label(this, 10, 500, 480 - 20, 50, "Send SMS");
        sendButton->setAlignment(gui::Alignment(gui::Alignment::Vertical::Center, gui::Alignment::Horizontal::Center));
        sendButton->setPenWidth(1);
        sendButton->setPenFocusWidth(3);
        sendButton->activatedCallback = [=](gui::Item &item) {
            LOG_INFO("Send SMS callback");
            CellularServiceAPI::SendSMS(application, sendNumber->getText(), sendMessage->getText());
            return true;
        };

        sendButton->setNavigationItem(gui::NavigationDirection::UP, sendNumber);
        sendNumber->setNavigationItem(gui::NavigationDirection::UP, sendMessage);

        sendMessage->setNavigationItem(gui::NavigationDirection::DOWN, sendNumber);
        sendNumber->setNavigationItem(gui::NavigationDirection::DOWN, sendButton);
    }
    void TestMessageWindow::destroyInterface()
    {
        erase();
    }

    void TestMessageWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        setFocusItem(sendButton);
    }

    void TestMessageWindow::cellularMessageCallback(UTF8 &number, UTF8 &message)
    {
        receiveNumber->setText(number);
        receiveMessage->setText(message);
        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);
    }
} /* namespace gui */
