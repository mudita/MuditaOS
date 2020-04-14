/*
 * PhonebookOptions.cpp
 *
 *  Created on: Oct 22, 2019
 *      Author: atom
 */

#include "PhonebookOptionsNamecard.hpp"
#include "../ApplicationPhonebook.hpp"
#include "InputEvent.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "PhonebookContact.hpp"
#include "Text.hpp"
#include "application-call/data/CallSwitchData.hpp"
#include "i18/i18.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include <log/log.hpp>

PhonebookOptionsNamecard::PhonebookOptionsNamecard(app::Application *app) : AppWindow(app, "OptionsNamecard")
{
    buildInterface();
}

PhonebookOptionsNamecard::~PhonebookOptionsNamecard()
{}

gui::Item *PhonebookOptionsNamecard::addOptionLabel(const std::string &text,
                                                    bool hasSubOptions,
                                                    std::function<bool(gui::Item &)> activatedCallback)
{
    gui::Label *label = new gui::Label(this, 19, 0, 480 - 21, 55, text);
    label->setMargins(gui::Margins(0, 0, 0, 0));
    label->setFilled(false);
    label->setPenFocusWidth(3);
    label->setPenWidth(0);
    label->setFont(style::window::font::small);
    label->setAlignment(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_CENTER));
    label->activatedCallback = activatedCallback;
    label->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
    if (hasSubOptions) {
        new gui::Image(label, 427, 20, 32, 32, "right_label_arrow");
    }
    return label;
}

void PhonebookOptionsNamecard::buildInterface()
{
    AppWindow::buildInterface();

    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_select"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));

    topBar->setActive(TopBar::Elements::SIGNAL, true);
    topBar->setActive(TopBar::Elements::BATTERY, true);

    setTitle(utils::localize.get("app_phonebook_options_title"));

    options.push_back(addOptionLabel(utils::localize.get("app_phonebook_options_send_bt"), false, [=](gui::Item &item) {
        sendViaBluetooth();
        return (true);
    }));

    options.push_back(addOptionLabel(utils::localize.get("app_phonebook_options_send_sms"), false, [=](gui::Item &) {
        sendViaSms();
        return (true);
    }));

    // set position and navigation for labels
    uint32_t posY = 113;
    uint32_t size = options.size();
    for (uint32_t i = 0; i < options.size(); i++) {
        options[i]->setPosition(17, posY);
        posY += 60;
        options[i]->setNavigationItem(NavigationDirection::DOWN, options[(i + 1) % size]);
        options[i]->setNavigationItem(NavigationDirection::UP, options[(size + i - 1) % size]);
    }

    dbg = new Text(this, 0, 240, 480, 600 - 240);
    dbg->setFilled(false);
    dbg->setPenFocusWidth(0);
    dbg->setPenWidth(0);
    dbg->setBorderColor(gui::ColorFullBlack);
    dbg->setUnderline(false);
    dbg->setTextType(Text::TextType::MULTI_LINE);
    dbg->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
}

void PhonebookOptionsNamecard::onBeforeShow(ShowMode mode, SwitchData *data)
{
    setFocusItem(options[0]);
}

bool PhonebookOptionsNamecard::handleSwitchData(SwitchData *data)
{
    if (data == nullptr) {
        LOG_ERROR("Received null pointer");
        return false;
    }

    PhonebookItemData *item = dynamic_cast<PhonebookItemData *>(data);
    contact                 = item->getContact();
    return (true);
}

bool PhonebookOptionsNamecard::onInput(const InputEvent &inputEvent)
{
    if (inputEvent.keyCode == KeyCode::KEY_RF && (inputEvent.state == InputEvent::State::keyReleasedShort)) {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
        application->switchWindow("Options", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
        return true;
    }

    return (AppWindow::onInput(inputEvent));
}

void PhonebookOptionsNamecard::sendViaSms()
{
    const std::string vcard = formatVCARD();
    dbg->setText(vcard);
}

void PhonebookOptionsNamecard::sendViaBluetooth()
{
    const std::string vcard = formatVCARD();
    dbg->setText(vcard);
}

const std::string PhonebookOptionsNamecard::formatVCARD()
{
    const std::string priNumber = (contact->numbers.size() > 0) ? contact->numbers[0].numberE164.c_str() : "";
    const std::string secNumber = (contact->numbers.size() > 1) ? contact->numbers[1].numberE164.c_str() : "";
    std::ostringstream vcard;
    vcard << "BEGIN:VCARD\n"
          << "VERSION:3.0\n"
          << "N:" << contact->alternativeName.c_str() << ";" << contact->primaryName.c_str() << ";;;\n"
          << "FN:" << contact->primaryName.c_str() << " " << contact->alternativeName.c_str() << "\n"
          << "TEL;TYPE=HOME,VOICE:" << priNumber << "\n";

    if (secNumber.length() > 0) {
        vcard << "TEL;TYPE=HOME,VOICE:" << secNumber << "\n";
    }

    vcard << "ADR;TYPE=HOME:;;" << contact->street.c_str() << ";" << contact->city.c_str() << "\n"
          << "EMAIL:" << contact->mail.c_str() << "\n"
          << "END:VCARD";

    LOG_INFO("formatted vcard:");
    LOG_INFO("%s", vcard.str().c_str());
    return (vcard.str());
}
