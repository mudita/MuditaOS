// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OptionsMessages.hpp"
#include "application-messages/data/SMSdata.hpp"
#include "tools/Common.hpp"

#include <common_data/Clipboard.hpp>
#include <Options.hpp>
#include <OptionWindow.hpp>
#include <i18/i18.hpp>
#include <log/log.hpp>

#include <Text.hpp>
#include <BoxLayout.hpp>
#include <memory>
#include <module-services/service-db/service-db/DBServiceAPI.hpp>

using namespace style::window;

std::list<gui::Option> smsWindowOptions(app::ApplicationMessages *app, const SMSRecord &record)
{
    ContactRecord contact = DBServiceAPI::ContactGetByIDWithTemporary(app, record.contactID)->front();
    std::list<gui::Option> options;

    if (record.type == SMSType::FAILED) {
        options.emplace_back(utils::localize.get("sms_resend_failed"), [=, &record](gui::Item &item) {
            app->resendSms(record);
            app->returnToPreviousWindow();
            return true;
        });
    }
    options.push_back(gui::options::call(app, contact));
    auto contactOperation =
        contact.isTemporary() ? gui::options::ContactOperation::Add : gui::options::ContactOperation::Details;
    options.push_back(gui::options::contact(app, contactOperation, contact));
    options.emplace_back(UTF8(utils::localize.get("sms_forward_message")), [=](gui::Item &item) {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<SMSTextData>(record.body);
        app->switchWindow(gui::name::window::new_sms, std::move(data));
        return true;
    });
    options.emplace_back(UTF8(utils::localize.get("sms_copy")), [=](gui::Item &item) {
        Clipboard::getInstance().copy(record.body);
        app->returnToPreviousWindow();
        return true;
    });
    options.emplace_back(UTF8(utils::localize.get("sms_delete_message")),
                         [=](gui::Item &item) { return app->removeSms(record); });

    return options;
}

std::list<gui::Option> newMessageWindowOptions(app::ApplicationMessages *app,
                                               const std::string &requestingWindow,
                                               gui::Text *text)
{
    std::list<gui::Option> options;

    options.emplace_back(UTF8(utils::localize.get("sms_use_template")), [=](gui::Item &item) {
        std::unique_ptr<gui::SwitchData> data = std::make_unique<SMSTemplateRequest>(requestingWindow);
        app->switchWindow(gui::name::window::sms_templates, std::move(data));
        return true;
    });

    if (Clipboard::getInstance().gotData()) {
        options.emplace_back(utils::localize.get("sms_paste"), [=](gui::Item &item) {
            text->addText(Clipboard::getInstance().paste());
            app->returnToPreviousWindow();
            return true;
        });
    }

    return options;
}
