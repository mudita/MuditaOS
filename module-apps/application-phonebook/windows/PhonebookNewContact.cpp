﻿// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhonebookNewContact.hpp"

#include "DialogMetadata.hpp"
#include "DialogMetadataMessage.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"

#include <Dialog.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-db/DBServiceAPI.hpp>

namespace gui
{

    PhonebookNewContact::PhonebookNewContact(app::ApplicationCommon *app)
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

        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::save));
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));

        setTitle(utils::translate("app_phonebook_contact_title"));

        list = new gui::ListView(this,
                                 phonebookStyle::newContactWindow::newContactsList::x,
                                 phonebookStyle::newContactWindow::newContactsList::y,
                                 phonebookStyle::newContactWindow::newContactsList::w,
                                 phonebookStyle::newContactWindow::newContactsList::h,
                                 newContactModel,
                                 listview::ScrollBarType::PreRendered);
        list->setBoundaries(Boundaries::Continuous);
        newContactModel->createData();

        setFocusItem(list);
    }

    void PhonebookNewContact::destroyInterface()
    {
        erase();
    }

    void PhonebookNewContact::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        saveInfoAboutPreviousAppForProperSwitchBack(data);

        if (mode == ShowMode::GUI_SHOW_INIT && data != nullptr) {
            newContactModel->loadData(contact, data);
            list->rebuildList();
        }

        switch (contactAction) {
        case ContactAction::None:
            break;
        case ContactAction::Add:
        case ContactAction::EditTemporary:
            setTitle(utils::translate("app_phonebook_contact_title"));
            break;
        case ContactAction::Edit:
            setTitle(utils::translate("app_phonebook_options_edit"));
            break;
        }

        setSaveButtonVisible(!newContactModel->readyToSave());
    }

    void PhonebookNewContact::onClose(Window::CloseReason reason)
    {
        if (reason == Window::CloseReason::ApplicationClose) {
            newContactModel->clearData();
        }
    }

    auto PhonebookNewContact::handleSwitchData(SwitchData *data) -> bool
    {
        if (data == nullptr) {
            return false;
        }

        const auto item = dynamic_cast<PhonebookItemData *>(data);
        if (item == nullptr) {
            return false;
        }

        contact = item->getContact();
        if (contact == nullptr) {
            contactAction = ContactAction::Add;
            contact       = std::make_shared<ContactRecord>();
            return true;
        }

        if (contact->ID == DB_ID_NONE) {
            contactAction = ContactAction::Add;
        }
        else if (contact->isTemporary()) {
            contactAction = ContactAction::EditTemporary;
        }
        else {
            contactAction = ContactAction::Edit;
        }

        return true;
    }

    void PhonebookNewContact::showDialogUnsavedChanges(const std::function<bool()> &whereToGoOnYes)
    {
        // Show a popup warning about possible data loss
        auto metaData = std::make_unique<gui::DialogMetadataMessage>(
            gui::DialogMetadata{utils::translate("unsaved_changes"),
                                "delete_128px_W_G",
                                utils::translate("exit_without_saving"),
                                "",
                                [=]() -> bool {
                                    application->returnToPreviousWindow(); // To exit this popup
                                    return whereToGoOnYes();
                                }});

        application->switchWindow(gui::window::name::dialog_yes_no, std::move(metaData));
    }

    void PhonebookNewContact::setSaveButtonVisible(bool visible)
    {
        navBar->setActive(nav_bar::Side::Center, visible);
    }

    auto PhonebookNewContact::onInput(const InputEvent &inputEvent) -> bool
    {
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER) && !newContactModel->emptyData() &&
            newContactModel->verifyData()) {
            auto tmpId  = contact->ID;
            contact     = std::make_shared<ContactRecord>();
            contact->ID = tmpId;

            newContactModel->saveData(contact);
            verifyAndSave();
            return true;
        }
        else if (!inputEvent.isShortRelease(KeyCode::KEY_RF) || !shouldCurrentAppBeIgnoredOnSwitchBack()) {
            if (isAnyUnsavedUserDataInWindow()) {
                if (inputEvent.isShortRelease(gui::KeyCode::KEY_RF) || inputEvent.isLongRelease(gui::KeyCode::KEY_RF)) {
                    showDialogUnsavedChanges([this]() {
                        application->returnToPreviousWindow();
                        return true;
                    });
                    return true;
                }
            }
            auto retunrVal = AppWindow::onInput(inputEvent);
            setSaveButtonVisible(!newContactModel->readyToSave());
            return retunrVal;
        }

        auto returnWhenCurrentAppShouldBeIgnoredOnSwitchBack = [this]() {
            return shouldCurrentAppBeIgnoredOnSwitchBack()
                       ? app::manager::Controller::switchBack(
                             application,
                             std::make_unique<app::manager::SwitchBackRequest>(nameOfPreviousApplication.value()))
                       : true;
        };

        if (isAnyUnsavedUserDataInWindow()) {
            showDialogUnsavedChanges(returnWhenCurrentAppShouldBeIgnoredOnSwitchBack);
            return true;
        }
        return returnWhenCurrentAppShouldBeIgnoredOnSwitchBack();
    }

    auto PhonebookNewContact::verifyAndSave() -> bool
    {
        if (!contact->isTemporary()) {
            auto result = DBServiceAPI::verifyContact(application, *contact);
            switch (result) {
            case DBServiceAPI::ContactVerificationResult::success:
                break;
            case DBServiceAPI::ContactVerificationResult::emptyContact:
                return false;
            case DBServiceAPI::ContactVerificationResult::primaryNumberDuplicate:
                showDialogDuplicatedNumber(contact->numbers[0].number, contact->ID);
                return false;
            case DBServiceAPI::ContactVerificationResult::secondaryNumberDuplicate:
                showDialogDuplicatedNumber(contact->numbers[1].number, contact->ID);
                return false;
            case DBServiceAPI::ContactVerificationResult::primaryAndSecondaryNumberAreTheSame:
                showDialogPrimaryAndSecondaryNumberAreTheSame();
                return false;
            case DBServiceAPI::ContactVerificationResult::speedDialDuplicate:
                showDialogDuplicatedSpeedDialNumber();
                return false;
            case DBServiceAPI::ContactVerificationResult::temporaryContactExists:
                LOG_DEBUG("Temporary contact exists. Let's update the contact and unbind the phone numbers from the "
                          "temporary one");
                break;
            }
        }
        else {
            contact->removeFromGroup(ContactsDB::temporaryGroupId());
        }
        // perform actual add/update operation
        if (contactAction == ContactAction::Add) {
            auto returnedContact = DBServiceAPI::ContactAdd(application, *contact);
            if (!returnedContact.has_value()) {
                LOG_ERROR("Failed to add contact");
                return false;
            }
            *contact = returnedContact.value();
        }
        else if (contactAction == ContactAction::Edit || contactAction == ContactAction::EditTemporary) {
            contact->groups.erase(ContactsDB::temporaryGroupId());
            if (checkIfContactWasDeletedDuringEditProcess()) {
                LOG_WARN("Contact removed while editing by external resources.");
                showContactDeletedNotification();
                return false;
            }
            else if (!DBServiceAPI::ContactUpdate(application, *contact)) {
                LOG_ERROR("Failed to update contact");
                return false;
            }
        }

        auto switchData = std::make_unique<PhonebookItemData>(contact, newContactModel->getRequestType());
        switchData->ignoreCurrentWindowOnStack = true;
        application->switchWindow(gui::window::name::contact, std::move(switchData));
        return true;
    } // namespace gui

    void PhonebookNewContact::showDialogDuplicatedNumber(const utils::PhoneNumber::View &duplicatedNumber,
                                                         const std::uint32_t duplicatedNumberContactID)
    {
        const auto matchedContact =
            DBServiceAPI::MatchContactByPhoneNumber(application, duplicatedNumber, duplicatedNumberContactID);
        const auto oldContactRecord = (matchedContact != nullptr) ? *matchedContact : ContactRecord{};

        auto metaData = std::make_unique<gui::DialogMetadataMessage>(gui::DialogMetadata{
            duplicatedNumber.getFormatted(),
            "info_128px_W_G",
            text::RichTextParser()
                .parse(utils::translate("app_phonebook_duplicate_numbers"),
                       nullptr,
                       gui::text::RichTextParser::TokenMap(
                           {{"$CONTACT_FORMATTED_NAME$", oldContactRecord.getFormattedName()}}))
                ->getText(),
            "",
            [=]() -> bool {
                if (contactAction == ContactAction::Add) {
                    contact->ID = oldContactRecord.ID;
                }
                if (!DBServiceAPI::ContactUpdate(application, *contact)) {
                    LOG_ERROR("Failed to update contact (id: %" PRIu32 ")", contact->ID);
                    return false;
                }

                /* Pop "Add contact" window from the stack so that clicking
                 * back button after saving the modified contact returns to
                 * contacts list, not to the "Add contact" window. */
                application->popWindow(gui::window::name::new_contact);

                /* Switch to contact details */
                auto switchData = std::make_unique<PhonebookItemData>(contact, newContactModel->getRequestType());
                switchData->ignoreCurrentWindowOnStack = true;
                application->switchWindow(gui::window::name::contact, std::move(switchData));

                return true;
            }});
        application->switchWindow(gui::window::name::dialog_yes_no, std::move(metaData));
    }

    void PhonebookNewContact::showDialogDuplicatedSpeedDialNumber()
    {
        const auto contactRecordsPtr = DBServiceAPI::ContactGetBySpeeddial(application, contact->speeddial);
        const auto oldContactRecord  = !contactRecordsPtr->empty() ? contactRecordsPtr->front() : ContactRecord{};

        if (contactAction == ContactAction::Add) {
            contact->ID = oldContactRecord.ID;
        }

        auto metaData = std::make_unique<gui::DialogMetadataMessage>(gui::DialogMetadata{
            text::RichTextParser()
                .parse(utils::translate("app_phonebook_duplicate_speed_dial_title"),
                       nullptr,
                       gui::text::RichTextParser::TokenMap(
                           {{"$CONTACT_SPEED_DIAL$", oldContactRecord.speeddial},
                            {"$CONTACT_FORMATTED_NAME$", oldContactRecord.getFormattedName()}}))
                ->getText(),
            "phonebook_empty_grey_circle_speed_dial",
            text::RichTextParser()
                .parse(utils::translate("app_phonebook_duplicate_speed_dial"),
                       nullptr,
                       gui::text::RichTextParser::TokenMap({{"$CONTACT_SPEED_DIAL$", oldContactRecord.speeddial}}))
                ->getText(),
            contact->speeddial,
            [=]() -> bool {
                if (!DBServiceAPI::ContactUpdate(application, *contact)) {
                    LOG_ERROR("Failed to update contact (id: %" PRIu32 ")", contact->ID);
                    return false;
                }
                application->switchWindow(gui::name::window::main_window);
                return true;
            }});
        application->switchWindow(gui::window::name::dialog_yes_no_icon_txt, std::move(metaData));
    }

    void PhonebookNewContact::showContactDeletedNotification()
    {
        auto metaData = std::make_unique<gui::DialogMetadataMessage>(gui::DialogMetadata{
            contact->getFormattedName(ContactRecord::NameFormatType::Title),
            "info_128px_W_G",
            utils::translate("app_phonebook_contact_deleted"),
            "",
            [=]() -> bool {
                auto switchData = std::make_unique<PhonebookItemData>(contact, newContactModel->getRequestType());
                application->switchWindow(gui::name::window::main_window, std::move(switchData));

                return true;
            }});
        application->switchWindow(gui::window::name::dialog_confirm, std::move(metaData));
    }

    bool PhonebookNewContact::checkIfContactWasDeletedDuringEditProcess() const
    {
        const auto contactByID = DBServiceAPI::ContactGetByID(application, contact->ID);

        return contactByID->size() == 1 and contactByID->front().ID == 0 and
               contactByID->front().primaryName.empty() and contactByID->front().alternativeName.empty() and
               contactByID->front().numbers.empty();
    }

    bool PhonebookNewContact::isAnyUnsavedUserDataInWindow() const
    {
        return newContactModel->isAnyUnsavedChange(contact);
    }

    void PhonebookNewContact::showDialogPrimaryAndSecondaryNumberAreTheSame()
    {
        auto metaData = std::make_unique<gui::DialogMetadataMessage>(
            gui::DialogMetadata{utils::translate("app_phonebook_new_contact_unable_to_save"),
                                "fail_128px_W_G",
                                utils::translate("app_phonebook_new_contact_same_numbers"),
                                "",
                                [=]() -> bool {
                                    application->returnToPreviousWindow();
                                    return true;
                                }});

        application->switchWindow(gui::window::name::dialog, std::move(metaData));
    }
} // namespace gui
