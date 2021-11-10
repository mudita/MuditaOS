// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ThreadWindowOptions.hpp"
#include "data/SMSdata.hpp"

#include <i18n/i18n.hpp>
#include <log/log.hpp>
#include <OptionCall.hpp>
#include <OptionContact.hpp>
#include <OptionWindow.hpp>

namespace gui
{

    ThreadWindowOptions::ThreadWindowOptions(app::ApplicationCommon *app, std::string windowName)
        : OptionWindow(app, windowName)
    {}

    void ThreadWindowOptions::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        if (auto message = dynamic_cast<SMSThreadData *>(data)) {
            thread = message->thread;
        }

        auto currentThreadValid = getThreadValid();

        if (threadValidCode == ValidCodes::Uninitialized || threadValidCode == currentThreadValid) {
            options = threadWindowOptions(dynamic_cast<app::ApplicationMessages *>(application), thread.get());
            optionsList->rebuildList(listview::RebuildType::InPlace);
        }
        else {
            application->returnToPreviousWindow();
        }

        threadValidCode = currentThreadValid;
    }

    ThreadWindowOptions::ValidCodes ThreadWindowOptions::getThreadValid()
    {
        auto contact = DBServiceAPI::MatchContactByNumberID(application, thread->numberID);
        return contact == nullptr ? ValidCodes::Invalid
                                  : (contact->isTemporary() ? ValidCodes::Invalid : ValidCodes::Valid);
    }

    std::list<Option> ThreadWindowOptions::threadWindowOptions(app::ApplicationMessages *app,
                                                               const ThreadRecord *record)
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
            options.emplace_back(gui::Option{
                std::make_unique<gui::option::Contact>(app, gui::option::ContactOperation::Add, newContact)});
        }
        else {
            options.emplace_back(
                gui::Option{std::make_unique<gui::option::Call>(app, contact->getFormattedName(), number)});
            options.emplace_back(gui::Option{
                std::make_unique<gui::option::Contact>(app, gui::option::ContactOperation::Details, *contact)});
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
    }

} // namespace gui
