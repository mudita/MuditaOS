/*
 * PhonebookDeleteContact.cpp
 *
 *  Created on: Oct 22, 2019
 *      Author: atom
 */

#include "PhonebookDialogs.hpp"
#include "../ApplicationPhonebook.hpp"
#include "InputEvent.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "PhonebookContact.hpp"
#include "PhonebookNewContact.hpp"
#include "Text.hpp"
#include "Utils.hpp"
#include "application-call/data/CallSwitchData.hpp"
#include "i18/i18.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include <log/log.hpp>
#include <sstream>

PhonebookDialog::PhonebookDialog(app::Application *app, const std::string &dialogName) : AppWindow(app, dialogName)
{
    LOG_INFO("PhonebookDialog::ctor %s", dialogName.c_str());
    buildInterface();
}

void PhonebookDialog::buildInterface()
{
    AppWindow::buildInterface();

    topBar->setActive(TopBar::Elements::TIME, true);

    bottomBar->setActive(BottomBar::Side::LEFT, false);
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);

    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_confirm"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

    icon = new Image(this, 176, 135, 128, 128, "phonebook_contact_delete_trashcan");

    confirmationLabel = new Text(this, 45, 293, 390, 128);
    confirmationLabel->setText(utils::localize.get("app_phonebook_options_delete_confirm"));
    confirmationLabel->setTextType(Text::TextType::MULTI_LINE);
    confirmationLabel->setEditMode(Text::EditMode::BROWSE);
    confirmationLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    confirmationLabel->setFont(style::window::font::small);

    noLabel = new Label(this, 75, 415, 150, 75, utils::localize.get("common_no"));
    noLabel->setPenWidth(0);
    noLabel->setPenFocusWidth(3);
    noLabel->setFilled(false);
    noLabel->setBorderColor(ColorFullBlack);
    noLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
    noLabel->setFont(style::window::font::small);
    noLabel->setAlignement(Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));

    yesLabel = new Label(this, 255, 415, 150, 75, utils::localize.get("common_yes"));
    yesLabel->setPenWidth(0);
    yesLabel->setPenFocusWidth(3);
    yesLabel->setFilled(false);
    yesLabel->setBorderColor(ColorFullBlack);
    yesLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
    yesLabel->setFont(style::window::font::small);
    yesLabel->setAlignement(Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));

    noLabel->setNavigationItem(NavigationDirection::DOWN, yesLabel);
    yesLabel->setNavigationItem(NavigationDirection::UP, noLabel);
}

bool PhonebookDialog::handleSwitchData(SwitchData *data)
{
    if (data == nullptr)
        return false;

    PhonebookItemData *item = dynamic_cast<PhonebookItemData *>(data);
    contact                 = item->getContact();

    setContactData();

    return (true);
}

bool PhonebookDialog::onInput(const InputEvent &inputEvent)
{
    if (inputEvent.keyCode == KeyCode::KEY_RF && (inputEvent.state == InputEvent::State::keyReleasedShort)) {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
        application->switchWindow("Options", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
        contact = nullptr;
        return (true);
    }

    return (AppWindow::onInput(inputEvent));
}

void PhonebookDeleteContact::onBeforeShow(ShowMode mode, SwitchData *data)
{
    LOG_INFO("PhonebookDeleteContact::initialize");
    icon = new Image(this, 176, 135, 128, 128, "phonebook_contact_delete_trashcan");

    noLabel->inputCallback = [=](gui::Item &item, const InputEvent &inputEvent) {
        if ((inputEvent.keyCode == KeyCode::KEY_ENTER) && ((inputEvent.state == InputEvent::State::keyReleasedShort) ||
                                                           (inputEvent.state == InputEvent::State::keyReleasedLong))) {
            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
            application->switchWindow("Options", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            return (true);
        }
        return (false);
    };

    yesLabel->inputCallback = [=](gui::Item &item, const InputEvent &inputEvent) {
        if ((inputEvent.keyCode == KeyCode::KEY_ENTER) && ((inputEvent.state == InputEvent::State::keyReleasedShort) ||
                                                           (inputEvent.state == InputEvent::State::keyReleasedLong))) {

            if (DBServiceAPI::ContactRemove(application, contact->dbID)) {
                LOG_INFO("contact %d removed, switch to MainWindow", contact->dbID);
            }
            else {
                LOG_ERROR("failed to delete contact with id %d", contact->dbID);
            }

            application->switchWindow(gui::name::window::main_window, gui::ShowMode::GUI_SHOW_INIT, nullptr);
            return (true);
        }
        return (false);
    };
}

void PhonebookDeleteContact::setContactData()
{
    setTitle(formatContactName(contact));
}

void PhonebookBlockContact::onBeforeShow(ShowMode mode, SwitchData *data)
{
    if (icon == nullptr)
        icon = new Image(this, 176, 135, 128, 128, "phonebook_blocked_large");

    confirmationLabel->setText(utils::localize.get("app_phonebook_options_block_confirm"));

    noLabel->inputCallback = [=](gui::Item &item, const InputEvent &inputEvent) {
        if ((inputEvent.keyCode == KeyCode::KEY_ENTER) && ((inputEvent.state == InputEvent::State::keyReleasedShort) ||
                                                           (inputEvent.state == InputEvent::State::keyReleasedLong))) {
            if (DBServiceAPI::ContactBlock(application, contact->dbID, false)) {
                LOG_INFO("contact %d unblocked, switch to MainWindow", contact->dbID);
            }
            else {
                LOG_ERROR("failed to unblock contact with id %d", contact->dbID);
            }

            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
            application->switchWindow("Options", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            return (true);
        }
        return (false);
    };

    yesLabel->inputCallback = [=](gui::Item &item, const InputEvent &inputEvent) {
        if ((inputEvent.keyCode == KeyCode::KEY_ENTER) && ((inputEvent.state == InputEvent::State::keyReleasedShort) ||
                                                           (inputEvent.state == InputEvent::State::keyReleasedLong))) {
            if (DBServiceAPI::ContactBlock(application, contact->dbID, true)) {
                LOG_INFO("contact %d blocked, switch to MainWindow", contact->dbID);
            }
            else {
                LOG_ERROR("failed to block contact with id %d", contact->dbID);
            }

            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
            application->switchWindow("Options", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            return (true);
        }
        return (false);
    };
}

void PhonebookBlockContact::setContactData()
{
    setTitle(formatContactName(contact));
}

DuplicatedContactDialogWindow::DuplicatedContactDialogWindow(app::Application *app)
    : Dialog(app,
             gui::window::name::duplicatedContact,
             {
                 .title  = "",
                 .icon   = "phonebook_info",
                 .text   = utils::localize.get("app_phonebook_duplicate_speed_dial"),
                 .action = []() -> bool {
                     LOG_INFO("!");
                     return true;
                 },
             })
{}

bool DuplicatedContactDialogWindow::handleSwitchData(SwitchData *data)
{
    if (data == nullptr) {
        return false;
    }

    PhonebookItemData *item = dynamic_cast<PhonebookItemData *>(data);
    if (item != nullptr) {
        auto record = item->getContact();

        auto meta   = this->meta;
        meta.action = [=]() -> bool {
            application->switchWindow(gui::window::name::newContact, std::make_unique<PhonebookItemData>(record));
            return true;
        };
        meta.title = item->text;
        meta.text  = updateText(meta.text, *record);
        this->update(meta);
        application->switchWindow(gui::window::name::duplicatedContact, nullptr);
        return true;
    }
    return false;
}

std::string DuplicatedContactDialogWindow::updateText(const std::string &text, const ContactRecord &rec)
{
    std::string str = text;
    fillContactData(str, std::make_shared<ContactRecord>(rec));
    return str;
}

void PhonebookDuplicateSpeedDial::onBeforeShow(ShowMode mode, SwitchData *data)
{
    if (icon == nullptr)
        icon = new Image(this, 176, 135, 128, 128, "phonebook_empty_grey_circle");

    confirmationLabel->setText(utils::localize.get("app_phonebook_duplicate_speed_dial"));

    dialValue = new Label(this, 176 + 32, 135 + 32, 64, 64);
    dialValue->setPenWidth(0);
    dialValue->setPenFocusWidth(0);
    dialValue->setFilled(false);
    dialValue->setBorderColor(ColorFullBlack);
    dialValue->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
    dialValue->setFont(style::window::font::verybig);
    dialValue->setAlignement(Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));

    noLabel->inputCallback = [=](gui::Item &item, const InputEvent &inputEvent) {
        if ((inputEvent.keyCode == KeyCode::KEY_ENTER) && ((inputEvent.state == InputEvent::State::keyReleasedShort) ||
                                                           (inputEvent.state == InputEvent::State::keyReleasedLong))) {
            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
            application->switchWindow(gui::window::name::newContact, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
        }
        return (false);
    };

    yesLabel->inputCallback = [=](gui::Item &item, const InputEvent &inputEvent) {
        if ((inputEvent.keyCode == KeyCode::KEY_ENTER) && ((inputEvent.state == InputEvent::State::keyReleasedShort) ||
                                                           (inputEvent.state == InputEvent::State::keyReleasedLong))) {
            if (contact != nullptr) {
                if (DBServiceAPI::ContactUpdate(application, *contact) == false) {
                    LOG_ERROR("can't update contact %s", contact->primaryName.c_str());
                }
                else {
                    LOG_INFO("Contact %s updated", contact->primaryName.c_str());
                }
            }
        }
        return (false);
    };
}

void PhonebookDuplicateSpeedDial::setContactData()
{
    /* for name formatting */
    std::string t = utils::localize.get("app_phonebook_duplicate_speed_dial");
    fillContactData(t, contact);
    confirmationLabel->setText(t);

    /* speeddial value convert to string */
    dialValue->setText(std::to_string(contact->speeddial));

    setTitle(contact.get()->numbers[0].numberE164);
}
