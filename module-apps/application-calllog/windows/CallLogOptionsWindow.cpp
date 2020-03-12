#include "CallLogOptionsWindow.hpp"
#include "i18/i18.hpp"
#include "log/log.hpp"
#include <Options.hpp>

/// below just for apps names...

std::list<gui::Option> calllogWindowOptions(app::ApplicationCallLog *app, const CalllogRecord &record)
{
    auto searchResults = DBServiceAPI::ContactGetByID(app, record.getContactId());

    std::list<gui::Option> options;

    if (searchResults.get()->empty() || searchResults->front().contactType == ContactType::TEMPORARY)
    {
        // add option - add contact
        options.push_back(gui::options::contact(app, app::ContactOperation::Add, searchResults->front()));
    }
    else
    {
        // add option - contact details
        options.push_back(gui::options::contact(app, app::ContactOperation::Details, searchResults->front()));
    }

    // add option delete call option
    options.push_back(gui::Option(
        utils::localize.get("app_calllog_options_delete_call"),
        [=](gui::Item &item) {
            auto application = dynamic_cast<app::ApplicationCallLog *>(app);
            if (application != nullptr)
                return application->removeCalllogEntry(record);
            return false;
        },
        gui::Arrow::Disabled));

    return options;
};
