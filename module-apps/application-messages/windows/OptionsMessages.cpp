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

using namespace style::window;

std::list<gui::Item *> smsWindowOptions(app::ApplicationMessages *app, const SMSRecord &record)
{
    ContactRecord contact = DBServiceAPI::ContactGetByIDWithTemporary(app, record.contactID)->front();
    std::list<gui::Item *> options;

    if (record.type == SMSType::FAILED) {
        options.push_back(gui::Option{
            utils::localize.get("sms_resend_failed"), [=, &record](gui::Item &item) {
                app->resendSms(record);
                app->returnToPreviousWindow();
                return true;
            }}.build());
    }
    options.push_back(gui::options::call(app, app::CallOperation::ExecuteCall, contact).build());
    auto contactOperation = contact.isTemporrary() ? app::ContactOperation::Add : app::ContactOperation::Details;
    options.push_back(gui::options::contact(app, contactOperation, contact).build());
    options.push_back(gui::Option{
        UTF8(utils::localize.get("sms_forward_message")), [=](gui::Item &item) {
            std::unique_ptr<gui::SwitchData> data = std::make_unique<SMSTextData>(record.body);
            app->switchWindow(gui::name::window::new_sms, std::move(data));
            return true;
        }}.build());
    options.push_back(gui::Option{
        UTF8(utils::localize.get("sms_copy")), [=](gui::Item &item) {
            Clipboard::getInstance().copy(record.body);
            app->returnToPreviousWindow();
            return true;
        }}.build());
    options.push_back(gui::Option{
        UTF8(utils::localize.get("sms_delete_message")), [=](gui::Item &item) {
            return app->removeSMS(record);
        }}.build());

    return options;
}

std::list<gui::Item *> newMessageWindowOptions(app::ApplicationMessages *app,
                                               const std::string &requestingWindow,
                                               gui::Text *text)
{
    std::list<gui::Item *> options;

    options.push_back(gui::Option{
        UTF8(utils::localize.get("sms_use_template")), [=](gui::Item &item) {
            std::unique_ptr<gui::SwitchData> data = std::make_unique<SMSTemplateRequest>(requestingWindow);
            app->switchWindow(gui::name::window::sms_templates, std::move(data));
            return true;
        }}.build());

    if (Clipboard::getInstance().gotData()) {
        options.push_back(gui::Option{
            utils::localize.get("sms_paste"), [=](gui::Item &item) {
                text->addText(Clipboard::getInstance().paste());
                app->returnToPreviousWindow();
                return true;
            }}.build());
    }

    return options;
}
