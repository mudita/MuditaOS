#include "Options.hpp"
#include <UiCommonActions.hpp>
#include <cassert>
#include <i18/i18.hpp>

namespace app
{
    gui::Option callOption(Application *app, CallOperation callOperation, const ContactRecord &contact)
    {
        assert(app != nullptr);
        return {UTF8(utils::localize.get("sms_call_text")) + contact.primaryName,
                [app, contact, callOperation](gui::Item &item) { return call(app, callOperation, contact); }, gui::Arrow::Enabled};
    }

    gui::Option contactOption(Application *app, ContactOperation contactOperation, const ContactRecord &contactRec, gui::Arrow arrow)
    {
        assert(app != nullptr);

        std::string str;
        switch (contactOperation)
        {
        case ContactOperation::Details:
            str = utils::localize.get("app_options_contact_details");
            break;

        case ContactOperation::Add:
            str = utils::localize.get("app_options_contact_add");
            break;

        case ContactOperation::Edit:
            str = utils::localize.get("app_options_contact_edit");
            break;

        default:
            str = utils::localize.get("app_options_invalid_option");
            LOG_WARN("ContactOperation %d not supported", static_cast<int>(contactOperation));
            break;
        }

        return {str, [=](gui::Item &item) { return contact(app, contactOperation, contactRec); }, arrow};
    }
} // namespace app
