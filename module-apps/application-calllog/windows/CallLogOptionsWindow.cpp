// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CallLogOptionsWindow.hpp"
#include <i18n/i18n.hpp>
#include <OptionContact.hpp>

/// below just for apps names...

std::list<gui::Option> calllogWindowOptions(app::ApplicationCallLog *app, const CalllogRecord &record)
{
    std::list<gui::Option> options;
    if (record.presentation != PresentationType::PR_UNKNOWN) {
        auto searchResults = DBServiceAPI::ContactGetByIDWithTemporary(app, record.getContactId());

        if (searchResults->empty() || !searchResults->front().isValid() || searchResults->front().isTemporary()) {
            // add option - add contact
            options.emplace_back(gui::Option{std::make_unique<gui::option::Contact>(
                app, gui::option::ContactOperation::Add, searchResults->front())});
        }
        else {
            // add option - contact details
            options.emplace_back(gui::Option{std::make_unique<gui::option::Contact>(
                app, gui::option::ContactOperation::Details, searchResults->front())});
        }
    }

    // add option delete call option
    options.push_back(gui::Option(
        utils::translate("app_calllog_options_delete_call"),
        [=](gui::Item &item) { return app->removeCalllogEntry(record); },
        gui::option::Arrow::Disabled));

    return options;
};
