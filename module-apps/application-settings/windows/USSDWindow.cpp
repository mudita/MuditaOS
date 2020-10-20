// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "USSDWindow.hpp"
#include "../ApplicationSettings.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "i18/i18.hpp"
#include "messages/AppMessage.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include <GridLayout.hpp>
#include <Style.hpp>
#include <functional>
#include <memory>
#include <Font.hpp>
#include "service-cellular/api/CellularServiceAPI.hpp"

namespace gui
{

    USSDWindow::USSDWindow(app::Application *app) : AppWindow(app, gui::window::name::ussd_window)
    {
        buildInterface();
    }

    void USSDWindow::rebuild()
    {}

    void USSDWindow::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        topBar->setActive(TopBar::Elements::SIGNAL, true);
        topBar->setActive(TopBar::Elements::BATTERY, true);

        setTitle("USSD test");

        uint32_t h = style::settings::ussd::commonYPos;

        incomingLabel = addTitle("Incoming code");
        incomingLabel->setPosition(style::settings::ussd::commonXPos, h);
        h += style::settings::ussd::commonLabelH;

        received = addText();

        received->setPosition(style::settings::ussd::commonXPos, h);
        h += style::settings::ussd::commonTextH;

        outgoingLabel = addTitle("Outgoing code");
        outgoingLabel->setPosition(style::settings::ussd::commonXPos, h);
        h += style::settings::ussd::commonLabelH;

        pullToSend = addText();
        pullToSend->setPosition(style::settings::ussd::commonXPos, h);
        pullToSend->activatedCallback = [=](gui::Item &) {
            std::string data = pullToSend->getText().c_str();
            CellularServiceAPI::USSDRequest(
                this->application, CellularUSSDMessage::RequestType::pullSesionRequest, data);
            return true;
        };
        pullToSend->setEditMode(EditMode::EDIT);
        pullToSend->setInputMode(new InputMode(
            {InputMode::digit},
            [=](const UTF8 &text) { bottomBarTemporaryMode(text); },
            [=]() { bottomBarRestoreFromTemporaryMode(); },
            [=]() { selectSpecialCharacter(); }));
        setFocusItem(pullToSend);
    }

    void USSDWindow::destroyInterface()
    {
        erase();
    }

    void USSDWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {}

    gui::Label *USSDWindow::addTitle(const UTF8 &title)
    {
        gui::Label *label = new gui::Label(this,
                                           style::settings::ussd::commonXPos,
                                           style::settings::ussd::commonYPos,
                                           style::settings::ussd::commonW,
                                           style::settings::ussd::commonLabelH,
                                           title);
        label->setFilled(false);
        label->setPenFocusWidth(style::window::default_border_focus_w);
        label->setPenWidth(style::window::default_border_no_focus_w);

        label->setFont(style::window::font::verysmall);
        label->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

        return label;
    }
    gui::Text *USSDWindow::addText(void)
    {
        gui::Text *text = new gui::Text(this,
                                        style::settings::ussd::commonXPos,
                                        style::settings::ussd::commonYPos,
                                        style::settings::ussd::commonW,
                                        style::settings::ussd::commonTextH);
        text->setFilled(false);
        text->setPenFocusWidth(style::window::default_border_focus_w);
        text->setPenWidth(style::window::default_border_no_focus_w);

        text->setFont(style::window::font::verysmall);
        text->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        text->setEdges(RectangleEdge::All);
        return text;
    }

    void USSDWindow::handleIncomingUSSD(const std::string &data)
    {
        received->setText(UTF8(data));
    }
} // namespace gui
