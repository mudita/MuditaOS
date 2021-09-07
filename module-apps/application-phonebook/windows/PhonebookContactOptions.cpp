// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhonebookContactOptions.hpp"
#include "DialogMetadata.hpp"
#include "DialogMetadataMessage.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "application-phonebook/data/PhonebookItemData.hpp"
#include "Dialog.hpp"

#include <memory>
#include <service-db/DBServiceAPI.hpp>

namespace gui
{

    PhonebookContactOptions::PhonebookContactOptions(app::Application *app)
        : OptionWindow(app, gui::window::name::contact_options)
    {}

    auto PhonebookContactOptions::handleSwitchData(SwitchData *data) -> bool
    {
        auto *item = dynamic_cast<PhonebookItemData *>(data);
        if (item == nullptr) {
            LOG_WARN("Received null pointer");
            return false;
        }
        contact = item->getContact();
        clearOptions();
        addOptions(contactOptionsList());

        return true;
    }

    auto PhonebookContactOptions::contactOptionsList() -> std::list<gui::Option>
    {
        std::list<gui::Option> options;
        options.emplace_back(gui::Option{utils::translate("app_phonebook_options_edit"), [=](gui::Item &item) {
                                             LOG_INFO("Editing contact!");
                                             std::unique_ptr<gui::SwitchData> data =
                                                 std::make_unique<PhonebookItemData>(contact);
                                             this->application->switchWindow(gui::window::name::new_contact,
                                                                             gui::ShowMode::GUI_SHOW_INIT,
                                                                             std::move(data));
                                             return true;
                                         }});
        options.emplace_back(gui::Option{utils::translate("app_phonebook_options_forward_namecard"),
                                         [=](gui::Item &item) {
                                             LOG_INFO("Forwarding namecard!");
                                             std::unique_ptr<gui::SwitchData> data =
                                                 std::make_unique<PhonebookItemData>(contact);
                                             this->application->switchWindow(gui::window::name::namecard_options,
                                                                             gui::ShowMode::GUI_SHOW_INIT,
                                                                             std::move(data));
                                             return true;
                                         },
                                         gui::option::Arrow::Enabled});
        options.emplace_back(gui::Option{utils::translate("app_phonebook_options_delete"), [=](gui::Item &item) {
                                             LOG_INFO("Deleting contact!");
                                             return contactRemove();
                                         }});
        return options;
    }

    auto PhonebookContactOptions::contactRemove() -> bool
    {
        LOG_DEBUG("Removing contact: %" PRIu32, contact->ID);
        std::unique_ptr<DialogMetadata> meta = std::make_unique<DialogMetadata>();

        auto contactRec = DBServiceAPI::ContactGetByID(this->application, contact->ID);
        auto cont       = !contactRec->empty() ? contactRec->front() : ContactRecord{};

        auto metaData = std::make_unique<gui::DialogMetadataMessage>(
            gui::DialogMetadata{cont.getFormattedName(),
                                "phonebook_contact_delete_trashcan",
                                utils::translate("app_phonebook_options_delete_confirm"),
                                "",
                                [=]() -> bool {
                                    if (!DBServiceAPI::ContactRemove(this->application, contact->ID)) {
                                        LOG_ERROR("Contact id=%" PRIu32 "  remove failed", contact->ID);
                                        return false;
                                    }
                                    showNotification(NotificationType::Delete);
                                    return true;
                                }});

        application->switchWindow(gui::window::name::dialog_yes_no, std::move(metaData));
        return true;
    }

    auto PhonebookContactOptions::showNotification(NotificationType notificationType) -> bool
    {
        std::string dialogText;

        switch (notificationType) {
        case NotificationType::Block:
            dialogText = utils::translate("app_phonebook_options_block_notification");
            break;
        case NotificationType::Delete:
            dialogText = utils::translate("app_phonebook_options_delete_notification");
            break;
        case NotificationType::Unblock:
            dialogText = utils::translate("app_phonebook_options_unblock_notification");
            break;
        }

        auto metaData = std::make_unique<gui::DialogMetadataMessage>(
            gui::DialogMetadata{contact->getFormattedName(ContactRecord::NameFormatType::Title),
                                "info_big_circle_W_G",
                                dialogText,
                                "",
                                [=]() -> bool {
                                    this->application->switchWindow(gui::name::window::main_window);
                                    return true;
                                }});

        application->switchWindow(gui::window::name::dialog_confirm, std::move(metaData));
        return true;
    }
} // namespace gui
