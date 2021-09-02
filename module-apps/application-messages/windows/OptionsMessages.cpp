// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SMSdata.hpp"

#include <apps-common/options/type/OptionCall.hpp>
#include <apps-common/options/type/OptionContact.hpp>
#include <Clipboard.hpp>
#include <i18n/i18n.hpp>
#include <Option.hpp>
#include <OptionsMessages.hpp>
#include <Text.hpp>
#include <tools/Common.hpp>

#include <memory>

std::list<gui::Option> smsWindowOptions(app::ApplicationMessages *app, const SMSRecord &record)
{
    ContactRecord contact = DBServiceAPI::ContactGetByIDWithTemporary(app, record.contactID)->front();
    std::list<gui::Option> options;

    if (record.type == SMSType::FAILED) {
        options.emplace_back(utils::translate("sms_resend_failed"), [=, &record](gui::Item &item) {
            app->resendSms(record);
            app->returnToPreviousWindow();
            return true;
        });
    }

    options.emplace_back(gui::Option{std::make_unique<gui::option::Call>(app, contact)});

    auto contactOperation =
        contact.isTemporary() ? gui::option::ContactOperation::Add : gui::option::ContactOperation::Details;
    options.emplace_back(gui::Option{std::make_unique<gui::option::Contact>(app, contactOperation, contact)});

    options.emplace_back(UTF8(utils::translate("sms_forward_message")), [=](gui::Item &item) {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<SMSTextData>(record.body);
        app->switchWindow(gui::name::window::new_sms, std::move(data));
        return true;
    });

    options.emplace_back(UTF8(utils::translate("sms_copy")), [=](gui::Item &item) {
        Clipboard::getInstance().copy(record.body);
        app->returnToPreviousWindow();
        return true;
    });

    options.emplace_back(UTF8(utils::translate("sms_delete_message")),
                         [=](gui::Item &item) { return app->removeSms(record); });

    return options;
}

std::list<gui::Option> newMessageWindowOptions(app::ApplicationMessages *app,
                                               const std::string &requestingWindow,
                                               gui::Text *text)
{
    std::list<gui::Option> options;

    options.emplace_back(UTF8(utils::translate("sms_use_template")), [=](gui::Item &item) {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<SMSTemplateRequest>(requestingWindow);
        app->switchWindow(gui::name::window::sms_templates, std::move(data));
        return true;
    });

    if (Clipboard::getInstance().gotData()) {
        options.emplace_back(utils::translate("sms_paste"), [=](gui::Item &item) {
            text->addText(Clipboard::getInstance().paste());
            app->returnToPreviousWindow();
            return true;
        });
    }

    return options;
}
