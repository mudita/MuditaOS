#include "OptionsWindow.hpp"
#include "i18/i18.hpp"
#include "segger/log/log.hpp"
#include <Options.hpp>

/// below just for apps names...

std::list<gui::Option> threadWindowOptions(app::ApplicationMessages *app, const ThreadRecord *record)
{

    ContactRecord contact = record ? DBServiceAPI::ContactGetByID(app, record->contactID)->front() : ContactRecord();

    return {
        gui::options::call(app, app::CallOperation::ExecuteCall, contact),
        gui::options::contact(app, app::ContactOperation::Details, contact),
        {utils::localize.get("sms_delete_conversation"),
         [=](gui::Item &item) {
             LOG_INFO("Removing sms thread!");
             return app->removeSMS_thread(record);
         },
         gui::Arrow::Disabled},

        // TODO
        // last sms, all sms? what author had in mind?
        // {utils::localize.get("sms_mark_read_unread"), [=](gui::Item &item) { return true; }},

        // TODO
        // shouldn't this be in show contact details actually? it would be much easier too
        // {utils::localize.get("sms_add_to_contacts"), [=](gui::Item &item) { return true; }},
    };
};
