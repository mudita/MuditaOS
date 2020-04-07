/*
 * PhonebookOptions.cpp
 *
 *  Created on: Oct 22, 2019
 *      Author: atom
 */

#include "PhonebookOptions.hpp"
#include "../ApplicationPhonebook.hpp"
#include "InputEvent.hpp"
#include "Label.hpp"
#include "Margins.hpp"
#include "PhonebookContact.hpp"
#include "PhonebookNewContact.hpp"
#include "Text.hpp"
#include "application-call/data/CallSwitchData.hpp"
#include "i18/i18.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include <log/log.hpp>

PhonebookOptions::PhonebookOptions(app::Application *app) : AppWindow(app, "Options")
{
    buildInterface();
}

PhonebookOptions::~PhonebookOptions()
{}

gui::Item *PhonebookOptions::addOptionLabel(const std::string &text,
                                            bool hasSubOptions,
                                            std::function<bool(gui::Item &)> activatedCallback)
{
    gui::Label *label = new gui::Label(this, 19, 0, 480 - 21, 55, text);
    label->setMargins(gui::Margins(0, 0, 0, 0));
    label->setFilled(false);
    label->setPenFocusWidth(3);
    label->setPenWidth(0);
    label->setFont(style::window::font::small);
    label->setAlignement(gui::Alignment(gui::Alignment::ALIGN_HORIZONTAL_LEFT, gui::Alignment::ALIGN_VERTICAL_CENTER));
    label->activatedCallback = activatedCallback;
    label->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);
    if (hasSubOptions) {
        new gui::Image(label, 427, 20, 32, 32, "right_label_arrow");
    }
    return label;
}

void PhonebookOptions::buildInterface()
{
    AppWindow::buildInterface();

    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_select"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));

    topBar->setActive(TopBar::Elements::SIGNAL, true);
    topBar->setActive(TopBar::Elements::BATTERY, true);

    setTitle(utils::localize.get("app_phonebook_options_title"));

    topSeparatorLabel = new Label(this, 0, 105, 480, 1);
    topSeparatorLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);

    options.push_back(addOptionLabel(utils::localize.get("app_phonebook_options_edit"), false, [=](gui::Item &item) {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
        application->switchWindow(gui::window::name::newContact, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
        return (true);
    }));

    options.push_back(
        addOptionLabel(utils::localize.get("app_phonebook_options_forward_namecard"), true, [=](gui::Item &) {
            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
            application->switchWindow("OptionsNamecard", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
            return (true);
        }));

    options.push_back(addOptionLabel(utils::localize.get("app_phonebook_options_block"), false, [=](gui::Item &) {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
        application->switchWindow("Block", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
        return (true);
    }));

    options.push_back(addOptionLabel(utils::localize.get("app_phonebook_options_delete"), false, [=](gui::Item &) {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
        application->switchWindow("Delete", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
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
}

void PhonebookOptions::onBeforeShow(ShowMode mode, SwitchData *data)
{
    setFocusItem(options[0]);
}

bool PhonebookOptions::handleSwitchData(SwitchData *data)
{
    if (data == nullptr) {
        LOG_ERROR("Received null pointer");
        return false;
    }

    PhonebookItemData *item = dynamic_cast<PhonebookItemData *>(data);
    contact                 = item->getContact();
    return (true);
}

bool PhonebookOptions::onInput(const InputEvent &inputEvent)
{
    LOG_INFO("PhonebookOptions::onInput state: %d keyCode: %d",
             static_cast<int>(inputEvent.state),
             static_cast<int>(inputEvent.keyCode));
    if (inputEvent.keyCode == KeyCode::KEY_RF && inputEvent.state == InputEvent::State::keyReleasedShort) {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
        application->switchWindow("Contact", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
        return (true);
    }

    return (AppWindow::onInput(inputEvent));
}
