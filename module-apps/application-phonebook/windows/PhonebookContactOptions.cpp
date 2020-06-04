#include "PhonebookContactOptions.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "application-phonebook/data/PhonebookItemData.hpp"
#include "Dialog.hpp"

#include <service-db/api/DBServiceAPI.hpp>

namespace gui
{

    PhonebookContactOptions::PhonebookContactOptions(app::Application *app)
        : OptionWindow(app, gui::window::name::contact_options)
    {
        buildInterface();
        this->addOptions(contactOptionsList());
    }

    auto PhonebookContactOptions::handleSwitchData(SwitchData *data) -> bool
    {
        auto *item = dynamic_cast<PhonebookItemData *>(data);
        if (item == nullptr) {
            LOG_WARN("Received null pointer");
            return false;
        }
        contact = item->getContact();
        return true;
    }

    auto PhonebookContactOptions::contactOptionsList() -> std::list<gui::Option>
    {
        return {
            gui::Option{utils::localize.get("app_phonebook_options_edit"),
                        [=](gui::Item &item) {
                            LOG_INFO("Editing contact!");
                            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
                            this->application->switchWindow(
                                gui::window::name::new_contact, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
                            return true;
                        }},
            gui::Option{utils::localize.get("app_phonebook_options_forward_namecard"),
                        [=](gui::Item &item) {
                            LOG_INFO("Forwarding namecard!");
                            std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contact);
                            this->application->switchWindow(
                                gui::window::name::namecard_options, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
                            return true;
                        },
                        gui::Arrow::Enabled},
            gui::Option{utils::localize.get("app_phonebook_options_block"),
                        [=](gui::Item &item) {
                            LOG_INFO("Blocking contact!");
                            return contactBlock();
                        }},
            gui::Option{utils::localize.get("app_phonebook_options_delete"),
                        [=](gui::Item &item) {
                            LOG_INFO("Deleting contact!");
                            return contactRemove();
                        }},
        };
    }

    auto PhonebookContactOptions::contactBlock() -> bool
    {
        LOG_DEBUG("Blocking contact: %" PRIu32, contact->ID);
        auto dialog = dynamic_cast<gui::DialogYesNo *>(this->application->getWindow(gui::window::name::dialog_yes_no));
        assert(dialog != nullptr);
        auto meta   = dialog->meta;
        meta.action = [=]() -> bool {
            this->application->switchWindow(gui::name::window::main_window);
            return true;
        };
        meta.text       = utils::localize.get("app_phonebook_options_block_confirm");
        auto contactRec = DBServiceAPI::ContactGetByID(this->application, contact->ID);
        auto cont       = !contactRec->empty() ? contactRec->front() : ContactRecord{};
        meta.title      = cont.getFormattedName();
        meta.icon       = "phonebook_contact_delete_trashcan";
        dialog->update(meta);
        this->application->switchWindow(dialog->getName());
        return true;
    }

    auto PhonebookContactOptions::contactRemove() -> bool
    {
        LOG_DEBUG("Removing contact: %" PRIu32, contact->ID);
        auto dialog = dynamic_cast<gui::DialogYesNo *>(this->application->getWindow(gui::window::name::dialog_yes_no));
        assert(dialog != nullptr);
        auto meta   = dialog->meta;
        meta.action = [=]() -> bool {
            if (!DBServiceAPI::ContactRemove(this->application, contact->ID)) {
                LOG_ERROR("Contact id=%" PRIu32 "  remove failed", contact->ID);
                return false;
            }
            this->application->switchWindow(gui::name::window::main_window);
            return true;
        };
        meta.text       = utils::localize.get("app_phonebook_options_delete_confirm");
        auto contactRec = DBServiceAPI::ContactGetByID(this->application, contact->ID);
        auto cont       = !contactRec->empty() ? contactRec->front() : ContactRecord{};
        meta.title      = cont.getFormattedName();
        meta.icon       = "phonebook_contact_delete_trashcan";
        dialog->update(meta);
        this->application->switchWindow(dialog->getName());
        return true;
    }
} // namespace gui
