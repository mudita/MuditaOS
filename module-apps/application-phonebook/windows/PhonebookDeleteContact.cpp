/*
 * PhonebookDeleteContact.cpp
 *
 *  Created on: Oct 22, 2019
 *      Author: atom
 */

#include "PhonebookDeleteContact.hpp"
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

PhonebookDeleteContact::PhonebookDeleteContact(app::Application *app) : AppWindow(app, "Delete")
{
    setSize(480, 600);
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

    titleLabel = new Label(this, 0, 52, 480, 55);
    titleLabel->setFilled(false);
    titleLabel->setBorderColor(ColorFullBlack);
    titleLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
    titleLabel->setFont("gt_pressura_bold_24");
    titleLabel->setAlignement(Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));
    titleLabel->setLineMode(false);

    trashacnIcon = new Image(this, 176, 135, 128, 128, "trashcan_large");

    confirmationLabel = new Text(this, 45, 293, 390, 128);
    confirmationLabel->setText(utils::localize.get("app_phonebook_options_delete_confirm"));
    confirmationLabel->setTextType(Text::TextType::MULTI_LINE);
    confirmationLabel->setEditMode(Text::EditMode::BROWSE);
    confirmationLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
    // confirmationLabel->setAlignement(Alignment(Alignment::ALIGN_HORIZONTAL_CENTER,
    // Alignment::ALIGN_VERTICAL_BOTTOM));
    confirmationLabel->setFont("gt_pressura_regular_24");

    noLabel = new Label(this, 75, 415, 150, 75, utils::localize.get("common_no"));
    noLabel->setPenWidth(0);
    noLabel->setPenFocusWidth(3);
    noLabel->setFilled(false);
    noLabel->setBorderColor(ColorFullBlack);
    noLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
    noLabel->setFont("gt_pressura_regular_24");
    noLabel->setAlignement(Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));
    noLabel->inputCallback = [=](gui::Item &item, const InputEvent &inputEvent) {
        if ((inputEvent.keyCode == KeyCode::KEY_ENTER) && ((inputEvent.state == InputEvent::State::keyReleasedShort) ||
                                                           (inputEvent.state == InputEvent::State::keyReleasedLong)))
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
    yesLabel->setFont("gt_pressura_regular_24");
    yesLabel->setAlignement(Alignment(Alignment::ALIGN_HORIZONTAL_CENTER, Alignment::ALIGN_VERTICAL_CENTER));
    yesLabel->inputCallback = [=](gui::Item &item, const InputEvent &inputEvent) {
        if ((inputEvent.keyCode == KeyCode::KEY_ENTER) && ((inputEvent.state == InputEvent::State::keyReleasedShort) ||
                                                           (inputEvent.state == InputEvent::State::keyReleasedLong)))
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
    children.clear();
}

void PhonebookDeleteContact::onBeforeShow(ShowMode mode, SwitchData *data)
{
    setFocusItem(noLabel);
    application->setKeyboardProfile(utils::localize.get("common_kbd_upper"));
}

void PhonebookDeleteContact::setContactData()
{
    titleLabel->setText(contact->primaryName + " " + contact->alternativeName);
}

bool PhonebookDeleteContact::handleSwitchData(SwitchData *data)
{
    if (data == nullptr)
    {
        LOG_ERROR("Received null pointer");
        return false;
    }

    PhonebookItemData *item = reinterpret_cast<PhonebookItemData *>(data);
    contact = item->getContact();

    setContactData();

    return (true);
}

bool PhonebookDeleteContact::onInput(const InputEvent &inputEvent)
{
    // check if any of the lower inheritance onInput methods catch the event
    LOG_INFO("PhonebookContact::onInput");
    bool ret = AppWindow::onInput(inputEvent);

    if (ret)
    {
        // refresh window only when key is other than enter
        if (inputEvent.keyCode != KeyCode::KEY_ENTER)
        {
            application->render(RefreshModes::GUI_REFRESH_FAST);
        }
        return (true);
    }

    if ((inputEvent.state != InputEvent::State::keyReleasedShort) &&
        ((inputEvent.state != InputEvent::State::keyReleasedLong)))
    {
        return (false);
    }

    if (inputEvent.keyCode == KeyCode::KEY_ENTER)
    {
        LOG_INFO("Enter pressed");
    }
    else if (inputEvent.keyCode == KeyCode::KEY_LF)
    {
        LOG_INFO("Options");
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
    else
    {
        LOG_INFO("PhonebookContact::onInput unhandled event code=%d", inputEvent.keyCode);
    }
    return (false);
}
