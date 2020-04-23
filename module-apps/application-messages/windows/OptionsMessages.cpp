#include "OptionsMessages.hpp"
#include "Options.hpp"
#include "i18/i18.hpp"
#include "log/log.hpp"

#include <BoxLayout.hpp>

using namespace style::window;

gui::HBox *newCombo(app::ApplicationMessages *app, const ContactRecord &contact)
{
    const uint32_t wlabel = 64;
    const uint32_t offset = default_left_margin;

    auto box = new gui::HBox(nullptr, offset, 0, style::window_width - 2 * offset, label::big_h);
    box->setPenWidth(0);
    box->setPenFocusWidth(0);
    auto text_len = 200; // Dumb text size - this won't be needed when Text(...) widget will be refactored
    auto l        = new gui::Label(box, 0, 0, text_len, label::big_h);
    l->area(gui::Item::Area::Max).w = box->area().w; // let box layout resize
    decorate(l);
    l->setText(contact.getFormattedName());
    l->activeItem = false;

    auto p = [=](const UTF8 &icon, auto foo) {
        auto phone = new gui::Label(box, 0, 0, wlabel, label::big_h);
        decorate(phone);
        new gui::Image(phone, 12, 12, 50, label::big_h, icon);
        phone->activatedCallback = foo;
    };

    p("phonebook_phone_ringing", [=](gui::Item &) -> bool {
        LOG_INFO("Call: %s", contact.getFormattedName().c_str());
        return app::call(app, app::CallOperation::ExecuteCall, contact);
    });

    p("mail", [=](gui::Item &) -> bool {
        LOG_INFO("SMS to: %s", contact.getFormattedName().c_str());
        app::sms(app, app::SmsOperation::Add, contact);
        return true;
    });

    p("cross", [=](gui::Item &) -> bool {
        LOG_INFO("Add contact: %s", contact.getFormattedName().c_str());
        app::contact(app, app::ContactOperation::Add, contact);
        return true;
    });

    return box;
}

gui::Item *placeholder(const UTF8 &text)
{
    auto ret = new gui::Rect(nullptr,
                             default_left_margin,
                             0,
                             style::window_width - default_left_margin - default_right_margin,
                             label::small_h);
    auto t   = new gui::Label(ret, 0, 0, 0, 0, text);

    auto width = t->getTextNeedSpace() + 20;
    t->setSize(width, label::small_h);
    t->setPenWidth(0);

    auto r = new gui::Rect(ret, width, ret->getHeight() / 2, ret->getWidth() - width - 1, 2);
    r->setPenWidth(1);
    ret->setPenWidth(0);
    ret->activeItem = false;
    return ret;
}

std::list<gui::Item *> smsWindowOptions(app::ApplicationMessages *app, const SMSRecord &record)
{
    ContactRecord contact = DBServiceAPI::ContactGetByID(app, record.contactID)->front();
    return {
        gui::newOptionLabel(gui::options::call(app, app::CallOperation::ExecuteCall, contact)),
        gui::newOptionLabel(gui::options::contact(app, app::ContactOperation::Details, contact)),

        // TODO
        gui::newOptionLabel({UTF8(" <STUB> ") + UTF8(utils::localize.get("sms_forvard_message")),
                             [=](gui::Item &item) { return false; },
                             gui::Arrow::Disabled}),
        gui::newOptionLabel({UTF8(" <STUB> ") + UTF8(utils::localize.get("sms_copy")),
                             [=](gui::Item &item) { return false; },
                             gui::Arrow::Disabled}),
        gui::newOptionLabel({UTF8(" <STUB> ") + UTF8(utils::localize.get("sms_delete_message")),
                             [=](gui::Item &item) { return false; },
                             gui::Arrow::Disabled}),
        placeholder(utils::localize.get("sms_from_this_sms")),
        newCombo(app, contact) // contact.numbers[0].numberE164)
    };
}

std::list<gui::Item *> newMessageWindowOptions(app::ApplicationMessages *app)
{
    return {
        gui::newOptionLabel(
            {UTF8(utils::localize.get("sms_use_template")),
             [=](gui::Item &item) { return app->switchWindow(gui::name::window::sms_templates, nullptr); },
             gui::Arrow::Disabled}),
        gui::newOptionLabel({UTF8(" <STUB> ") + UTF8(utils::localize.get("sms_paste")),
                             [=](gui::Item &item) { return false; },
                             gui::Arrow::Disabled}),

    };
}
