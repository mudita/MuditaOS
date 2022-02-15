// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhonebookIceContacts.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "application-phonebook/data/PhonebookStyle.hpp"
#include "service-appmgr/Controller.hpp"

#include <service-db/DBNotificationMessage.hpp>

namespace gui
{
    PhonebookIceContacts::PhonebookIceContacts(app::ApplicationCommon *app)
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

        navBar->setActive(nav_bar::Side::Left, true);
        navBar->setActive(nav_bar::Side::Right, true);
        navBar->setText(nav_bar::Side::Left, utils::translate(style::strings::common::call));
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));
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
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            app::manager::Controller::switchBack(application);
            return true;
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
