#include "PhonebookContactOptions.hpp"
#include "PhonebookNewContact.hpp"
#include "../data/PhonebookItemData.hpp"
#include "i18/i18.hpp"
#include "log/log.hpp"
#include <Options.hpp>

std::list<gui::Option> contactOptions(app::ApplicationPhonebook *app, const uint32_t contactId)
{
    return {
        gui::Option{utils::localize.get("app_phonebook_options_edit"),
                    [=](gui::Item &item) {
                        LOG_INFO("Editing contact!");
                        std::shared_ptr<ContactRecord> contactSharedPtr(
                            new ContactRecord(DBServiceAPI::ContactGetByID(app, contactId)->front()));
                        std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contactSharedPtr);
                        app->switchWindow(gui::window::name::newContact, gui::ShowMode::GUI_SHOW_INIT, std::move(data));
                        return true;
                    }},
        gui::Option{utils::localize.get("app_phonebook_options_forward_namecard"),
                    [=](gui::Item &item) {
                        LOG_INFO("Forwarding namecard!");
                        std::shared_ptr<ContactRecord> contactSharedPtr(
                            new ContactRecord(DBServiceAPI::ContactGetByID(app, contactId)->front()));
                        std::unique_ptr<gui::SwitchData> data = std::make_unique<PhonebookItemData>(contactSharedPtr);
                        app->switchWindow("OptionsNamecard", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
                        return true;
                    },
                    gui::Arrow::Enabled},
        gui::Option{utils::localize.get("app_phonebook_options_block"),
                    [=](gui::Item &item) {
                        LOG_INFO("Blocking contact!");
                        return app->blockContact(contactId);
                    }},
        gui::Option{utils::localize.get("app_phonebook_options_delete"),
                    [=](gui::Item &item) {
                        LOG_INFO("Deleting contact!");
                        return app->removeContact(contactId);
                    }},
    };
}
