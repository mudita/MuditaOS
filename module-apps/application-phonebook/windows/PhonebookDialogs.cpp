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
#include "Text.hpp"
#include "Utils.hpp"
#include "application-call/data/CallSwitchData.hpp"
#include "i18/i18.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include <log/log.hpp>
#include <sstream>

PhonebookDeleteContact::PhonebookDeleteContact(app::Application *app) : AppWindow(app, "Delete")
{
    setSize(style::window_width, style::window_height);
    buildInterface();
}

PhonebookDeleteContact::~PhonebookDeleteContact()
{
    destroyInterface();
}

void PhonebookDeleteContact::rebuild()
{
    destroyInterface();
    buildInterface();
}

void PhonebookDeleteContact::buildInterface()
{
    AppWindow::buildInterface();
    topBar->setActive(TopBar::Elements::TIME, true);

    bottomBar->setActive(BottomBar::Side::LEFT, false);
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);

    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_confirm"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

    trashacnIcon = new Image(this, 176, 135, 128, 128, "phonebook_contact_delete_trashcan");

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
    noLabel->inputCallback = [=](gui::Item &item, const InputEvent &inputEvent) {
        if ((inputEvent.keyCode == KeyCode::KEY_ENTER) &&
            ((inputEvent.state == InputEvent::State::keyReleasedShort) || (inputEvent.state == InputEvent::State::keyReleasedLong)))
        {
            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
            application->switchWindow("Options", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            return (true);
        }
        return (false);
    };

    yesLabel = new Label(this, 255, 415, 150, 75, utils::localize.get("common_yes"));
    yesLabel->setPenWidth(0);
    yesLabel->setPenFocusWidth(3);
    yesLabel->setFilled(false);
    yesLabel->setBorderColor(ColorFullBlack);
    yesLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
    yesLabel->setFont(style::window::font::small);
    yesLabel->setAlignement(Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));
    yesLabel->inputCallback = [=](gui::Item &item, const InputEvent &inputEvent) {
        if ((inputEvent.keyCode == KeyCode::KEY_ENTER) &&
            ((inputEvent.state == InputEvent::State::keyReleasedShort) || (inputEvent.state == InputEvent::State::keyReleasedLong)))
        {

            if (DBServiceAPI::ContactRemove(application, contact->dbID))
            {
                LOG_INFO("contact %d removed, switch to MainWindow", contact->dbID);
            }
            else
            {
                LOG_ERROR("failed to delete contact with id %d", contact->dbID);
            }

            application->switchWindow("MainWindow", gui::ShowMode::GUI_SHOW_INIT, nullptr);
            return (true);
        }
        return (false);
    };

    noLabel->setNavigationItem(NavigationDirection::DOWN, yesLabel);
    yesLabel->setNavigationItem(NavigationDirection::UP, noLabel);
}

void PhonebookDeleteContact::destroyInterface()
{
    AppWindow::destroyInterface();
}

void PhonebookDeleteContact::onBeforeShow(ShowMode mode, SwitchData *data)
{
    setFocusItem(noLabel);
    application->setKeyboardProfile(utils::localize.get("common_kbd_upper"));
}

void PhonebookDeleteContact::setContactData()
{
    setTitle(formatContactName(contact));
}

bool PhonebookDeleteContact::handleSwitchData(SwitchData *data)
{
    if (data == nullptr)
        return false;

    PhonebookItemData *item = reinterpret_cast<PhonebookItemData *>(data);
    contact = item->getContact();

    setContactData();

    return (true);
}

bool PhonebookDeleteContact::onInput(const InputEvent &inputEvent)
{
    bool ret = AppWindow::onInput(inputEvent);

    if ((inputEvent.state != InputEvent::State::keyReleasedShort) && ((inputEvent.state != InputEvent::State::keyReleasedLong)))
    {
        return (false);
    }

    if (inputEvent.keyCode == KeyCode::KEY_LF)
    {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
        application->switchWindow("Options", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
        contact = nullptr;
        return (true);
    }
    else if (inputEvent.keyCode == KeyCode::KEY_RF)
    {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
        application->switchWindow("MainWindow", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
        contact = nullptr;
        return (true);
    }

    return (ret);
}

/**
 *
 */
PhonebookBlockContact::PhonebookBlockContact(app::Application *app) : AppWindow(app, "Block")
{
    setSize(style::window_width, style::window_height);
    buildInterface();
}

PhonebookBlockContact::~PhonebookBlockContact()
{
    destroyInterface();
}

void PhonebookBlockContact::rebuild()
{
    destroyInterface();
    buildInterface();
}

void PhonebookBlockContact::buildInterface()
{
    AppWindow::buildInterface();
    topBar->setActive(TopBar::Elements::TIME, true);

    bottomBar->setActive(BottomBar::Side::LEFT, false);
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);

    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_confirm"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

    icon = new Image(this, 176, 135, 128, 128, "phonebook_blocked_large");

    confirmationLabel = new Text(this, 45, 293, 390, 128);
    confirmationLabel->setText(utils::localize.get("app_phonebook_options_block_confirm"));
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
    noLabel->inputCallback = [=](gui::Item &item, const InputEvent &inputEvent) {
        if ((inputEvent.keyCode == KeyCode::KEY_ENTER) &&
            ((inputEvent.state == InputEvent::State::keyReleasedShort) || (inputEvent.state == InputEvent::State::keyReleasedLong)))
        {
            if (DBServiceAPI::ContactBlock(application, contact->dbID, false))
            {
                LOG_INFO("contact %d unblocked, switch to MainWindow", contact->dbID);
            }
            else
            {
                LOG_ERROR("failed to unblock contact with id %d", contact->dbID);
            }

            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
            application->switchWindow("Options", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            return (true);
        }
        return (false);
    };

    yesLabel = new Label(this, 255, 415, 150, 75, utils::localize.get("common_yes"));
    yesLabel->setPenWidth(0);
    yesLabel->setPenFocusWidth(3);
    yesLabel->setFilled(false);
    yesLabel->setBorderColor(ColorFullBlack);
    yesLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
    yesLabel->setFont(style::window::font::small);
    yesLabel->setAlignement(Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));
    yesLabel->inputCallback = [=](gui::Item &item, const InputEvent &inputEvent) {
        if ((inputEvent.keyCode == KeyCode::KEY_ENTER) &&
            ((inputEvent.state == InputEvent::State::keyReleasedShort) || (inputEvent.state == InputEvent::State::keyReleasedLong)))
        {
            if (DBServiceAPI::ContactBlock(application, contact->dbID, true))
            {
                LOG_INFO("contact %d blocked, switch to MainWindow", contact->dbID);
            }
            else
            {
                LOG_ERROR("failed to block contact with id %d", contact->dbID);
            }

            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
            application->switchWindow("Options", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            return (true);
        }
        return (false);
    };

    noLabel->setNavigationItem(NavigationDirection::DOWN, yesLabel);
    yesLabel->setNavigationItem(NavigationDirection::UP, noLabel);
}

void PhonebookBlockContact::destroyInterface()
{
    AppWindow::destroyInterface();
}

void PhonebookBlockContact::onBeforeShow(ShowMode mode, SwitchData *data)
{
    setFocusItem(noLabel);
    application->setKeyboardProfile(utils::localize.get("common_kbd_upper"));
}

void PhonebookBlockContact::setContactData()
{
    setTitle(formatContactName(contact));
}

bool PhonebookBlockContact::handleSwitchData(SwitchData *data)
{
    if (data == nullptr)
        return false;

    PhonebookItemData *item = reinterpret_cast<PhonebookItemData *>(data);
    contact = item->getContact();

    setContactData();

    return (true);
}

bool PhonebookBlockContact::onInput(const InputEvent &inputEvent)
{
    bool ret = AppWindow::onInput(inputEvent);

    if ((inputEvent.state != InputEvent::State::keyReleasedShort) && ((inputEvent.state != InputEvent::State::keyReleasedLong)))
    {
        return (false);
    }

    if (inputEvent.keyCode == KeyCode::KEY_LF)
    {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
        application->switchWindow("Options", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
        contact = nullptr;
        return (true);
    }
    else if (inputEvent.keyCode == KeyCode::KEY_RF)
    {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
        application->switchWindow("MainWindow", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
        contact = nullptr;
        return (true);
    }

    return (ret);
}

/**
 *
 */
PhonebookDuplicateNumber::PhonebookDuplicateNumber(app::Application *app) : AppWindow(app, "NumberAlreadyExists")
{
    setSize(style::window_width, style::window_height);
    buildInterface();
}

PhonebookDuplicateNumber::~PhonebookDuplicateNumber()
{
    destroyInterface();
}

void PhonebookDuplicateNumber::rebuild()
{
    destroyInterface();
    buildInterface();
}

void PhonebookDuplicateNumber::buildInterface()
{
    AppWindow::buildInterface();
    topBar->setActive(TopBar::Elements::TIME, true);

    bottomBar->setActive(BottomBar::Side::LEFT, false);
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);

    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_confirm"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

    icon = new Image(this, 176, 135, 128, 128, "phonebook_info");

    confirmationLabel = new Text(this, 45, 293, 390, 128);
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
    noLabel->inputCallback = [=](gui::Item &item, const InputEvent &inputEvent) {
        if ((inputEvent.keyCode == KeyCode::KEY_ENTER) &&
            ((inputEvent.state == InputEvent::State::keyReleasedShort) || (inputEvent.state == InputEvent::State::keyReleasedLong)))
        {
            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
            application->switchWindow("New", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
        }
        return (true);
    };

    yesLabel = new Label(this, 255, 415, 150, 75, utils::localize.get("common_yes"));
    yesLabel->setPenWidth(0);
    yesLabel->setPenFocusWidth(3);
    yesLabel->setFilled(false);
    yesLabel->setBorderColor(ColorFullBlack);
    yesLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
    yesLabel->setFont(style::window::font::small);
    yesLabel->setAlignement(Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));
    yesLabel->inputCallback = [=](gui::Item &item, const InputEvent &inputEvent) {
        if ((inputEvent.keyCode == KeyCode::KEY_ENTER) &&
            ((inputEvent.state == InputEvent::State::keyReleasedShort) || (inputEvent.state == InputEvent::State::keyReleasedLong)))
        {}
        return (false);
    };

    noLabel->setNavigationItem(NavigationDirection::DOWN, yesLabel);
    yesLabel->setNavigationItem(NavigationDirection::UP, noLabel);
}

void PhonebookDuplicateNumber::destroyInterface()
{
    AppWindow::destroyInterface();
}

void PhonebookDuplicateNumber::onBeforeShow(ShowMode mode, SwitchData *data)
{
    setFocusItem(noLabel);
    application->setKeyboardProfile(utils::localize.get("common_kbd_upper"));
}

void PhonebookDuplicateNumber::setContactData()
{
    std::string t = utils::localize.get("app_phonebook_duplicate_numbers");
    fillContactData(t, contact);
    confirmationLabel->setText(t);
    setTitle(contact.get()->numbers[0].numberE164);
}

bool PhonebookDuplicateNumber::handleSwitchData(SwitchData *data)
{
    if (data == nullptr)
        return false;

    PhonebookItemData *item = reinterpret_cast<PhonebookItemData *>(data);
    contact = item->getContact();

    setContactData();

    return (true);
}

bool PhonebookDuplicateNumber::onInput(const InputEvent &inputEvent)
{
    bool ret = AppWindow::onInput(inputEvent);

    if ((inputEvent.state != InputEvent::State::keyReleasedShort) && ((inputEvent.state != InputEvent::State::keyReleasedLong)))
    {
        return (false);
    }

    if (inputEvent.keyCode == KeyCode::KEY_LF)
    {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
        application->switchWindow("Options", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
        contact = nullptr;
        return (true);
    }
    else if (inputEvent.keyCode == KeyCode::KEY_RF)
    {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
        application->switchWindow("MainWindow", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
        contact = nullptr;
        return (true);
    }

    return (ret);
}

/**
 *
 */
PhonebookDuplicateSpeedDial::PhonebookDuplicateSpeedDial(app::Application *app) : AppWindow(app, "SpeedDialAlreadyAssigned")
{
    setSize(style::window_width, style::window_height);
    buildInterface();
}

PhonebookDuplicateSpeedDial::~PhonebookDuplicateSpeedDial()
{
    destroyInterface();
}

void PhonebookDuplicateSpeedDial::rebuild()
{
    destroyInterface();
    buildInterface();
}

void PhonebookDuplicateSpeedDial::buildInterface()
{
    AppWindow::buildInterface();
    topBar->setActive(TopBar::Elements::TIME, true);

    bottomBar->setActive(BottomBar::Side::LEFT, false);
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);

    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_confirm"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

    icon = new Image(this, 176, 135, 128, 128, "phonebook_empty_grey_circle");

    confirmationLabel = new Text(this, 45, 293, 390, 128);
    confirmationLabel->setTextType(Text::TextType::MULTI_LINE);
    confirmationLabel->setEditMode(Text::EditMode::BROWSE);
    confirmationLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    confirmationLabel->setFont(style::window::font::small);

    dialValue = new Label(this, 176 + 32, 135 + 32, 64, 64);
    dialValue->setPenWidth(0);
    dialValue->setPenFocusWidth(0);
    dialValue->setFilled(false);
    dialValue->setBorderColor(ColorFullBlack);
    dialValue->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
    dialValue->setFont(style::window::font::verybig);
    dialValue->setAlignement(Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));

    noLabel = new Label(this, 75, 415, 150, 75, utils::localize.get("common_no"));
    noLabel->setPenWidth(0);
    noLabel->setPenFocusWidth(3);
    noLabel->setFilled(false);
    noLabel->setBorderColor(ColorFullBlack);
    noLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
    noLabel->setFont(style::window::font::small);
    noLabel->setAlignement(Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));
    noLabel->inputCallback = [=](gui::Item &item, const InputEvent &inputEvent) {
        if ((inputEvent.keyCode == KeyCode::KEY_ENTER) &&
            ((inputEvent.state == InputEvent::State::keyReleasedShort) || (inputEvent.state == InputEvent::State::keyReleasedLong)))
        {
            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
            application->switchWindow("New", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
        }
        return (false);
    };

    yesLabel = new Label(this, 255, 415, 150, 75, utils::localize.get("common_yes"));
    yesLabel->setPenWidth(0);
    yesLabel->setPenFocusWidth(3);
    yesLabel->setFilled(false);
    yesLabel->setBorderColor(ColorFullBlack);
    yesLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
    yesLabel->setFont(style::window::font::small);
    yesLabel->setAlignement(Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));
    yesLabel->inputCallback = [=](gui::Item &item, const InputEvent &inputEvent) {
        if ((inputEvent.keyCode == KeyCode::KEY_ENTER) &&
            ((inputEvent.state == InputEvent::State::keyReleasedShort) || (inputEvent.state == InputEvent::State::keyReleasedLong)))
        {
            if (contact != nullptr)
            {
                if (DBServiceAPI::ContactUpdate(application, *contact) == false)
                {
                    LOG_ERROR("can't update contact %s", contact->primaryName.c_str());
                }
                else
                {
                    LOG_INFO("Contact %s updated", contact->primaryName.c_str());
                }
            }
        }
        return (false);
    };

    noLabel->setNavigationItem(NavigationDirection::DOWN, yesLabel);
    yesLabel->setNavigationItem(NavigationDirection::UP, noLabel);
}

void PhonebookDuplicateSpeedDial::destroyInterface()
{
    AppWindow::destroyInterface();
}

void PhonebookDuplicateSpeedDial::onBeforeShow(ShowMode mode, SwitchData *data)
{
    setFocusItem(noLabel);
    application->setKeyboardProfile(utils::localize.get("common_kbd_upper"));
}

void PhonebookDuplicateSpeedDial::setContactData()
{
    /* for name formatting */
    std::string t = utils::localize.get("app_phonebook_duplicate_speed_dial");
    fillContactData(t, contact);
    confirmationLabel->setText(t);

    /* speeddial value convert to string */
    dialValue->setText(itoa(contact->speeddial));

    setTitle(contact.get()->numbers[0].numberE164);
}

bool PhonebookDuplicateSpeedDial::handleSwitchData(SwitchData *data)
{
    if (data == nullptr)
        return false;

    PhonebookItemData *item = reinterpret_cast<PhonebookItemData *>(data);
    contact = item->getContact();

    setContactData();

    return (true);
}

bool PhonebookDuplicateSpeedDial::onInput(const InputEvent &inputEvent)
{
    bool ret = AppWindow::onInput(inputEvent);

    if ((inputEvent.state != InputEvent::State::keyReleasedShort) && ((inputEvent.state != InputEvent::State::keyReleasedLong)))
    {
        return (false);
    }

    if (inputEvent.keyCode == KeyCode::KEY_LF)
    {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
        application->switchWindow("Options", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
        contact = nullptr;
        return (true);
    }
    else if (inputEvent.keyCode == KeyCode::KEY_RF)
    {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
        application->switchWindow("MainWindow", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
        contact = nullptr;
        return (true);
    }

    return (ret);
}
