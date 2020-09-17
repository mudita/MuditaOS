#include "PhonebookIceContacts.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "application-phonebook/data/PhonebookStyle.hpp"

#include <module-services/service-db/messages/DBNotificationMessage.hpp>

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

        topBar->setActive(TopBar::Elements::TIME, true);
        setTitle(utils::localize.get("app_phonebook_ice_contacts_title"));

        contactsListIce = new gui::ListView(this,
                                            phonebookStyle::iceContactsWindow::contactsListIce::x,
                                            phonebookStyle::iceContactsWindow::contactsListIce::y,
                                            phonebookStyle::iceContactsWindow::contactsListIce::w,
                                            phonebookStyle::iceContactsWindow::contactsListIce::h,
                                            phonebookModel);

        setFocusItem(contactsListIce);

        bottomBar->setActive(BottomBar::Side::LEFT, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get(style::strings::common::call));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
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
} /* namespace gui */
