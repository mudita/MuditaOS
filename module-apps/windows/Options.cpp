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
                [app, contact, callOperation](gui::Item &item) { return call(app, callOperation, contact); }};
    }

    gui::Option contactOption(Application *app, ContactOperation contactOperation, const ContactRecord &contactRec)
    {
        assert(app != nullptr);

        return {utils::localize.get("sms_contact_details"), [=](gui::Item &item) { return contact(app, contactOperation, contactRec); }};
    }
} // namespace app
