#include "OptionsMessages.hpp"
#include "UiCommon.hpp"
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
    auto l = new gui::Label(box, 0, 0, text_len, label::big_h);
    l->area(gui::Item::Area::Max).w = box->w(); // let box layout resize
    decorate(l);
    l->setText(contact.primaryName);
    l->activeItem = false;

    auto p = [=](const UTF8 &icon, auto foo) {
        auto phone = new gui::Label(box, 0, 0, wlabel, label::big_h);
        decorate(phone);
        new gui::Image(phone, 12, 12, 50, label::big_h, icon);
        phone->activatedCallback = foo;
    };

    p("phonebook_phone_ringing", [=](gui::Item &) -> bool {
        LOG_INFO("Call: %s", contact.primaryName);
        return call(app, contact);
    });

    p("mail", [=](gui::Item &) -> bool {
        LOG_INFO("SMS to: %s", contact.primaryName);
        sms(app, contact);
        return true;
    });

    p("cross", [=](gui::Item &) -> bool {
        LOG_INFO("Add contact: %s", contact.primaryName);
        addContact(app, contact);
        return true;
    });

    return box;
}

gui::Item *placeholder(const UTF8 &text)
{
    auto ret = new gui::Rect(nullptr, default_left_margin, 0, style::window_width - default_left_margin - default_right_margin, label::small_h);
    auto t = new gui::Label(ret, 0, 0, 0, 0, text);

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
        gui::newOptionLabel(callOption(app, contact, true)), gui::newOptionLabel(contactDetails(app, contact)),

        // TODO
        gui::newOptionLabel({UTF8(" <STUB> ") + UTF8(utils::localize.get("sms_forvard_message")), [=](gui::Item &item) { return false; }}),
        gui::newOptionLabel({UTF8(" <STUB> ") + UTF8(utils::localize.get("sms_copy")), [=](gui::Item &item) { return false; }}),
        gui::newOptionLabel({UTF8(" <STUB> ") + UTF8(utils::localize.get("sms_delete_message")), [=](gui::Item &item) { return false; }}),
        placeholder(utils::localize.get("sms_from_this_sms")), newCombo(app, contact) // contact.numbers[0].numberE164)
    };
}
