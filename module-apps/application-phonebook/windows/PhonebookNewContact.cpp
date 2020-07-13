#include "PhonebookNewContact.hpp"

#include "application-phonebook/ApplicationPhonebook.hpp"
#include "PhonebookContact.hpp"

#include <Dialog.hpp>
#include <service-db/api/DBServiceAPI.hpp>

namespace gui
{

    PhonebookNewContact::PhonebookNewContact(app::Application *app)
        : AppWindow(app, gui::window::name::new_contact), newContactModel{std::make_shared<NewContactModel>(app)}
    {
        buildInterface();
    }

    void PhonebookNewContact::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void PhonebookNewContact::buildInterface()
    {
        AppWindow::buildInterface();

        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::save));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        topBar->setActive(TopBar::Elements::TIME, true);

        setTitle(utils::localize.get("app_phonebook_contact_title"));

        list = new gui::ListView(this,
                                 phonebookStyle::mainWindow::newContactsList::x,
                                 phonebookStyle::mainWindow::newContactsList::y,
                                 phonebookStyle::mainWindow::newContactsList::w,
                                 phonebookStyle::mainWindow::newContactsList::h,
                                 newContactModel);
        setFocusItem(list);
    }

    void PhonebookNewContact::destroyInterface()
    {
        erase();
    }

    void PhonebookNewContact::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (mode != ShowMode::GUI_SHOW_RETURN) {
            newContactModel->clearData();
        }

        if (mode == ShowMode::GUI_SHOW_INIT) {
            list->setElementsCount(newContactModel->getItemCount());
        }

        switch (contactAction) {
        case ContactAction::None:
            break;
        case ContactAction::Add:
            setTitle(utils::localize.get("app_phonebook_contact_title"));
            break;
        case ContactAction::Edit:
            setTitle(utils::localize.get("app_phonebook_options_edit"));
            break;
        }

        newContactModel->loadData(contact);
    }

    auto PhonebookNewContact::handleSwitchData(SwitchData *data) -> bool
    {
        if (data == nullptr) {
            return false;
        }

        auto *item = dynamic_cast<PhonebookItemData *>(data);
        if (item == nullptr) {
            return false;
        }

        contact = item->getContact();
        if (contact == nullptr) {
            contactAction = ContactAction::Add;
            contact       = std::make_shared<ContactRecord>();
            return true;
        }

        if (contact->ID == DB_ID_NONE || contact->contactType == ContactType::TEMPORARY) {
            contactAction = ContactAction::Add;
        }
        else {
            contactAction = ContactAction::Edit;
        }

        return true;
    }

    auto PhonebookNewContact::onInput(const InputEvent &inputEvent) -> bool
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        if (inputEvent.state != InputEvent::State::keyReleasedShort) {
            return false;
        }

        if (inputEvent.keyCode == gui::KeyCode::KEY_ENTER) {
            auto tmpId  = contact->ID;
            contact     = std::make_shared<ContactRecord>();
            contact->ID = tmpId;
            newContactModel->saveData(contact);
            verifyAndSave();
            return true;
        }

        application->refreshWindow(RefreshModes::GUI_REFRESH_FAST);

        return false;
    }

    auto PhonebookNewContact::verifyAndSave() -> bool
    {
        ContactRecord errNumPrim, errNumAlt, errSpeedDial;

        if (contactAction == ContactAction::Add) {
            DBServiceAPI::ContactVerificationError err =
                DBServiceAPI::verifyContact(application, *contact, errNumPrim, errNumAlt, errSpeedDial);
            LOG_INFO("Contact data verification result: \"%s\"", DBServiceAPI::getVerificationErrorString(err).c_str());
            switch (err) {
            case DBServiceAPI::noError:
                break;
            case DBServiceAPI::primaryNumberError:
                showDialogDuplicatedNumber(contact->numbers[0].number);
                return false;
            case DBServiceAPI::secondaryNumberError:
                showDialogDuplicatedNumber(contact->numbers[1].number);
                return false;
            case DBServiceAPI::speedDialError:
                showDialogDuplicatedSpeedDialNumber();
                return false;
            }

            if (DBServiceAPI::ContactAdd(application, *contact) == false) {
                LOG_ERROR("verifyAndSave failed to ADD contact");
                return false;
            }
            else {
                auto contactRec = DBServiceAPI::ContactGetByID(application, DBServiceAPI::ContactGetCount(application));
                assert(!contactRec->empty());
                contact                               = std::make_shared<ContactRecord>(contactRec->front());
                std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
                application->switchWindow(gui::window::name::contact, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
                LOG_INFO("verifyAndSave contact ADDED");
                return true;
            }
        }
        else if (contactAction == ContactAction::Edit) {
            if (contact->contactType == ContactType::TEMPORARY) {
                contact->contactType = ContactType::USER;
            }
            if (DBServiceAPI::ContactUpdate(application, *contact) == false) {
                LOG_ERROR("verifyAndSave failed to UPDATE contact");
                return false;
            }
            else {
                std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
                application->switchWindow(gui::window::name::contact, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
                LOG_INFO("verifyAndSave contact UPDATED");
                return true;
            }
        }
        return false;
    }

    void PhonebookNewContact::showDialogDuplicatedNumber(const utils::PhoneNumber::View &duplicatedNumber)
    {
        auto dialog = dynamic_cast<gui::DialogYesNo *>(application->getWindow(gui::window::name::dialog_yes_no));
        assert(dialog != nullptr);
        auto meta             = dialog->meta;
        auto matchedContact   = DBServiceAPI::MatchContactByPhoneNumber(application, duplicatedNumber);
        auto oldContactRecord = (matchedContact != nullptr) ? *matchedContact : ContactRecord{};
        contact->ID           = oldContactRecord.ID;
        meta.action           = [=]() -> bool {
            if (!DBServiceAPI::ContactUpdate(application, *contact)) {
                LOG_ERROR("Contact id=%" PRIu32 " update failed", contact->ID);
                return false;
            }
            application->switchWindow(gui::name::window::main_window);
            return true;
        };
        std::string duplicatedNumberPhrase = utils::localize.get("app_phonebook_duplicate_numbers");
        fillContactData(duplicatedNumberPhrase, oldContactRecord);
        meta.text  = duplicatedNumberPhrase;
        meta.title = duplicatedNumber.getFormatted();
        meta.icon  = "info_big_circle_W_G";
        dialog->update(meta);
        application->switchWindow(dialog->getName());
    }

    void PhonebookNewContact::showDialogDuplicatedSpeedDialNumber()
    {
        auto dialog = dynamic_cast<gui::DialogYesNo *>(application->getWindow(gui::window::name::dialog_yes_no));
        assert(dialog != nullptr);
        auto meta              = dialog->meta;
        auto contactRecordsPtr = DBServiceAPI::ContactGetBySpeeddial(application, contact->speeddial);
        auto oldContactRecord  = !contactRecordsPtr->empty() ? contactRecordsPtr->front() : ContactRecord{};
        contact->ID            = oldContactRecord.ID;
        meta.action            = [=]() -> bool {
            if (!DBServiceAPI::ContactUpdate(application, *contact)) {
                LOG_ERROR("Contact id=%" PRIu32 " update failed", contact->ID);
                return false;
            }
            application->switchWindow(gui::name::window::main_window);
            return true;
        };
        std::string duplicatedSpeedDialPhrase = utils::localize.get("app_phonebook_duplicate_numbers");
        fillContactData(duplicatedSpeedDialPhrase, oldContactRecord);
        std::string duplicatedSpeedDialTitle = utils::localize.get("app_phonebook_duplicate_speed_dial_title");
        fillContactData(duplicatedSpeedDialTitle, oldContactRecord);
        meta.text  = duplicatedSpeedDialPhrase;
        meta.title = duplicatedSpeedDialTitle;
        meta.icon  = "info_big_circle_W_G";
        dialog->update(meta);
        application->switchWindow(dialog->getName());
    }

} // namespace gui
