// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OptionContact.hpp"
#include "OptionStyle.hpp"
#include "FontManager.hpp"
#include "Text.hpp"

#include <service-appmgr/Controller.hpp>
#include <module-apps/application-phonebook/data/PhonebookItemData.hpp>

namespace gui::option
{
    Contact::Contact(app::ApplicationCommon *app,
                     ContactOperation contactOperation,
                     const ContactRecord &contactRec,
                     gui::option::Arrow arrow)
    {
        assert(app != nullptr);

        switch (contactOperation) {
        case ContactOperation::Details:
            text = utils::translate("app_options_contact_details");
            break;
        case ContactOperation::Add:
            text = utils::translate("app_options_contact_add");
            break;
        case ContactOperation::Edit:
            text = utils::translate("app_options_contact_edit");
            break;
        default:
            text = utils::translate("app_options_invalid_option");
            LOG_WARN("ContactOperation %d not supported", static_cast<int>(contactOperation));
            break;
        }

        activatedCallback = [=](gui::Item &) { return onContactOptionClick(app, contactOperation, contactRec); };
        this->arrow       = arrow;
    }

    bool Contact::onContactOptionClick(app::ApplicationCommon *app,
                                       ContactOperation contactOperation,
                                       const ContactRecord &contactRecord)
    {
        auto data = std::make_unique<PhonebookItemData>(std::make_shared<ContactRecord>(contactRecord));

        switch (contactOperation) {
        case ContactOperation::Add: {
            data->ignoreCurrentWindowOnStack = true;
            return app::manager::Controller::sendAction(app,
                                                        app::manager::actions::AddContact,
                                                        std::move(data),
                                                        app::manager::OnSwitchBehaviour::RunInBackground);
        }
        case ContactOperation::Details: {
            data->ignoreCurrentWindowOnStack = true;
            return app::manager::Controller::sendAction(app,
                                                        app::manager::actions::ShowContactDetails,
                                                        std::move(data),
                                                        app::manager::OnSwitchBehaviour::RunInBackground);
        }
        case ContactOperation::Edit: {
            return app::manager::Controller::sendAction(app,
                                                        app::manager::actions::EditContact,
                                                        std::move(data),
                                                        app::manager::OnSwitchBehaviour::RunInBackground);
        }
        }
        return false;
    }
} // namespace gui::option
