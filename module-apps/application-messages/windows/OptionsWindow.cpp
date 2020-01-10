#include "OptionsWindow.hpp"
#include "i18/i18.hpp"
#include "log/log.hpp"

#include <application-phonebook/data/PhonebookItemData.hpp>
#include <service-db/api/DBServiceAPI.hpp>

/// below just for apps names...
#include "application-call/ApplicationCall.hpp"
#include "application-call/data/CallSwitchData.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "service-appmgr/ApplicationManager.hpp"

std::list<gui::Option> threadWindowOptions(app::ApplicationMessages *app, const ThreadRecord *record)
{

    auto contact = record ? DBServiceAPI::ContactGetByID(app, record->contactID)->front() : ContactRecord();

    return {
        {UTF8(utils::localize.get("sms_call_text")) + contact.primaryName,
         [=](gui::Item &item) {
             if (record == nullptr)
             {
                 return false;
             }
             else
             {
                 std::unique_ptr<app::ExecuteCallData> data = std::make_unique<app::ExecuteCallData>(contact.numbers[0].numberE164.c_str());
                 return sapm::ApplicationManager::messageSwitchApplication(app, app::name_call, "CallWindow", std::move(data));
             }
         }},
        {utils::localize.get("sms_contact_details"),
         [=](gui::Item &item) {
             return sapm::ApplicationManager::messageSwitchApplication(
                 app, app::name_phonebook, "Contact", std::make_unique<PhonebookItemData>(std::shared_ptr<ContactRecord>(new ContactRecord(contact))));
         }},
        {utils::localize.get("sms_delete_conversation"),
         [=](gui::Item &item) {
             LOG_INFO("Removing sms thread!");
             return app->removeSMS_thread(record);
         }},

        // TODO
        // last sms, all sms? what author had in mind?
        // {utils::localize.get("sms_mark_read_unread"), [=](gui::Item &item) { return true; }},

        // TODO
        // shouldn't this be in show contact details actually? it would be much easier too
        // {utils::localize.get("sms_add_to_contacts"), [=](gui::Item &item) { return true; }},
    };
};
