// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ThreadWindowOptions.hpp"

#include <i18n/i18n.hpp>
#include <log/log.hpp>
#include <OptionCall.hpp>
#include <OptionContact.hpp>
#include <OptionWindow.hpp>

std::list<gui::Option> threadWindowOptions(app::ApplicationMessages *app, const ThreadRecord *record)
{
    assert(record != nullptr);
    auto number  = DBServiceAPI::NumberByID(app, record->numberID);
    auto contact = DBServiceAPI::MatchContactByNumberID(app, record->numberID);
    if (!contact->isValid()) {
        return {};
    }
    const auto isTempContact = contact->isTemporary();

    std::list<gui::Option> options;
    if (isTempContact) {
        options.emplace_back(gui::Option{std::make_unique<gui::option::Call>(app, number.getFormatted(), number)});

        ContactRecord newContact;
        newContact.numbers.emplace_back(number);
        options.emplace_back(
            gui::Option{std::make_unique<gui::option::Contact>(app, gui::option::ContactOperation::Add, newContact)});
    }
    else {
        options.emplace_back(
            gui::Option{std::make_unique<gui::option::Call>(app, contact->getFormattedName(), number)});
        options.emplace_back(
            gui::Option{std::make_unique<gui::option::Contact>(app, gui::option::ContactOperation::Details, *contact)});
    }

    if (record->isUnread()) {
        options.emplace_back(gui::Option{utils::translate("sms_mark_read"), [=](gui::Item &item) {
                                             app->markSmsThreadAsRead(record->ID);
                                             app->returnToPreviousWindow();
                                             return true;
                                         }});
    }
    else {
        options.emplace_back(gui::Option{utils::translate("sms_mark_unread"), [=](gui::Item &item) {
                                             app->markSmsThreadAsUnread(record->ID);
                                             app->returnToPreviousWindow();
                                             return true;
                                         }});
    }

    options.emplace_back(gui::Option{utils::translate("sms_delete_conversation"), [=](gui::Item &item) {
                                         LOG_INFO("Removing sms thread!");
                                         return app->removeSmsThread(record);
                                     }});

    // TODO
    // shouldn't this be in show contact details actually? it would be much easier too
    // {utils::translate("sms_add_to_contacts"), [=](gui::Item &item) { return true; }},

    return options;
};
