#include "PhonebookNewContact.hpp"

#include "application-phonebook/ApplicationPhonebook.hpp"
#include "application-phonebook/data/PhonebookUtils.hpp"

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

        setTitle(utils::localize.get("app_phonebook_contact_title"));

        list = new gui::ListView(this,
                                 phonebookStyle::newContactWindow::newContactsList::x,
                                 phonebookStyle::newContactWindow::newContactsList::y,
                                 phonebookStyle::newContactWindow::newContactsList::w,
                                 phonebookStyle::newContactWindow::newContactsList::h,
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
            list->rebuildList();
        }

        switch (contactAction) {
        case ContactAction::None:
            break;
        case ContactAction::Add:
        case ContactAction::EditTemporary:
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
            setSaveButtonVisible(false);
            return true;
        }

        if (contact->ID == DB_ID_NONE) {
            contactAction = ContactAction::Add;
            setSaveButtonVisible(false);
        }
        else if (contact->contactType == ContactType::TEMPORARY) {
            contactAction = ContactAction::EditTemporary;
        }
        else {
            contactAction = ContactAction::Edit;
            setSaveButtonVisible(true);
        }

        return true;
    }

    void PhonebookNewContact::setSaveButtonVisible(bool visible)
    {
        bottomBar->setActive(BottomBar::Side::CENTER, visible);
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
        auto err = DBServiceAPI::verifyContact(application, *contact);
        LOG_INFO("Contact data verification result: \"%s\"", DBServiceAPI::getVerificationErrorString(err).c_str());
        switch (err) {
        case DBServiceAPI::noError:
            break;
        case DBServiceAPI::emptyContactError:
            return false;
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

        // perform actual add/update operation
        if (contactAction == ContactAction::Add) {
            if (DBServiceAPI::ContactAdd(application, *contact) == false) {
                LOG_ERROR("verifyAndSave failed to ADD contact");
                return false;
            }
            LOG_DEBUG("verifyAndSave contact ADDED");
        }
        else if (contactAction == ContactAction::Edit || contactAction == ContactAction::EditTemporary) {
            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
            data->ignoreCurrentWindowOnStack      = true;
            if (contact->contactType == ContactType::TEMPORARY) {
                contact->contactType = ContactType::USER;
            }
            if (DBServiceAPI::ContactUpdate(application, *contact) == false) {
                LOG_ERROR("verifyAndSave failed to UPDATE contact");
                return false;
            }
            LOG_DEBUG("verifyAndSave contact UPDATED");
        }

        application->switchWindow(gui::name::window::main_window);
        return true;
    } // namespace gui

    void PhonebookNewContact::showDialogDuplicatedNumber(const utils::PhoneNumber::View &duplicatedNumber)
    {
        auto dialog = dynamic_cast<gui::DialogYesNo *>(application->getWindow(gui::window::name::dialog_yes_no));
        assert(dialog != nullptr);
        auto meta             = dialog->meta;
        auto matchedContact   = DBServiceAPI::MatchContactByPhoneNumber(application, duplicatedNumber);
        auto oldContactRecord = (matchedContact != nullptr) ? *matchedContact : ContactRecord{};

        if (contactAction == ContactAction::Add) {
            contact->ID = oldContactRecord.ID;
        }

        meta.action = [=]() -> bool {
            if (!DBServiceAPI::ContactUpdate(application, *contact)) {
                LOG_ERROR("Contact id=%" PRIu32 " update failed", contact->ID);
                return false;
            }
            application->switchWindow(gui::name::window::main_window);
            return true;
        };
        std::string duplicatedNumberPhrase = utils::localize.get("app_phonebook_duplicate_numbers");
        phonebookUtils::fillContactData(duplicatedNumberPhrase, oldContactRecord);
        meta.text  = duplicatedNumberPhrase;
        meta.title = duplicatedNumber.getFormatted();
        meta.icon  = "info_big_circle_W_G";
        dialog->update(meta);
        application->switchWindow(dialog->getName());
    }

    void PhonebookNewContact::showDialogDuplicatedSpeedDialNumber()
    {
        auto dialog =
            dynamic_cast<gui::DialogYesNoIconTxt *>(application->getWindow(gui::window::name::dialog_yes_no_icon_txt));
        assert(dialog != nullptr);
        auto meta              = dialog->meta;
        auto contactRecordsPtr = DBServiceAPI::ContactGetBySpeeddial(application, contact->speeddial);
        auto oldContactRecord  = !contactRecordsPtr->empty() ? contactRecordsPtr->front() : ContactRecord{};

        if (contactAction == ContactAction::Add) {
            contact->ID = oldContactRecord.ID;
        }

        meta.action = [=]() -> bool {
            if (!DBServiceAPI::ContactUpdate(application, *contact)) {
                LOG_ERROR("Contact id=%" PRIu32 " update failed", contact->ID);
                return false;
            }
            application->switchWindow(gui::name::window::main_window);
            return true;
        };
        std::string duplicatedSpeedDialPhrase = utils::localize.get("app_phonebook_duplicate_numbers");
        phonebookUtils::fillContactData(duplicatedSpeedDialPhrase, oldContactRecord);
        std::string duplicatedSpeedDialTitle = utils::localize.get("app_phonebook_duplicate_speed_dial_title");
        phonebookUtils::fillContactData(duplicatedSpeedDialTitle, oldContactRecord);
        meta.text  = duplicatedSpeedDialPhrase;
        meta.title = duplicatedSpeedDialTitle;
        meta.icon  = "phonebook_empty_grey_circle_speed_dial";
        dialog->SetIconText(contact->speeddial);

        dialog->update(meta);
        application->switchWindow(dialog->getName());
    }

} // namespace gui
