﻿// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhonebookIceContacts.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "application-phonebook/data/PhonebookStyle.hpp"

#include <service-db/DBNotificationMessage.hpp>

namespace gui
{
    PhonebookIceContacts::PhonebookIceContacts(app::Application *app)
        : AppWindow(app, gui::window::name::ice_contacts), phonebookModel{std::make_shared<PhonebookModel>(
                                                               this->application, "", ContactsDB::iceGroupId())}
    {
        buildInterface();
    }

    void PhonebookIceContacts::rebuild()
    {
        contactsListIce->rebuildList();
    }

    void PhonebookIceContacts::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::translate("app_phonebook_ice_contacts_title"));

        contactsListIce = new gui::ListView(this,
                                            phonebookStyle::iceContactsWindow::contactsListIce::x,
                                            phonebookStyle::iceContactsWindow::contactsListIce::y,
                                            phonebookStyle::iceContactsWindow::contactsListIce::w,
                                            phonebookStyle::iceContactsWindow::contactsListIce::h,
                                            phonebookModel,
                                            gui::listview::ScrollBarType::Fixed);

        setFocusItem(contactsListIce);

        bottomBar->setActive(BottomBar::Side::LEFT, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::LEFT, utils::translate(style::strings::common::call));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));
    }

    void PhonebookIceContacts::destroyInterface()
    {
        erase();
    }

    PhonebookIceContacts::~PhonebookIceContacts()
    {
        destroyInterface();
    }

    bool PhonebookIceContacts::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortPress()) {
            if (inputEvent.is(KeyCode::KEY_ENTER)) {
                return true;
            }
        }
        // check if any of the lower inheritance onInput methods catch the event
        return AppWindow::onInput(inputEvent);
    }

    bool PhonebookIceContacts::onDatabaseMessage(sys::Message *msgl)
    {
        auto *msgNotification = dynamic_cast<db::NotificationMessage *>(msgl);
        if (msgNotification != nullptr) {
            if (msgNotification->interface == db::Interface::Name::Contact) {

                if (msgNotification->dataModified()) {

                    rebuild();

                    return true;
                }
            }
        }

        return false;
    }

    void PhonebookIceContacts::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        rebuild();
    }
} /* namespace gui */
