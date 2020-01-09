#include "OptionsMessages.hpp"
#include "UiCommon.hpp"
#include "i18/i18.hpp"
#include "log/log.hpp"

#include <BoxLayout.hpp>

gui::HBox *newCombo(app::ApplicationMessages *app, const UTF8 &text)
{
    auto box = new gui::HBox(nullptr, 0, 0, style::window_width, style::window::label::big_h);
    box->setPenWidth(0);
    box->setPenFocusWidth(0);
    auto l = new gui::Label(box, 0, 0, 300, style::window::label::big_h);
    style::window::decorate(l);
    l->setText(text);
    l->activeItem = false;

    auto p = [=](const UTF8 &icon, auto foo) {
        auto phone = new gui::Label(box, 0, 0, 59, 50);
        style::window::decorate(phone);
        // does nothing for image :|
        phone->setAlignement({gui::Alignment::ALIGN_HORIZONTAL_CENTER | gui::Alignment::ALIGN_VERTICAL_CENTER});
        new gui::Image(phone, 10, 10, 50, style::window::label::big_h, icon);
        phone->activatedCallback = foo;
    };

    p("phonebook_phone_ringing", [=](gui::Item &) -> bool {
        LOG_INFO("Call: %s", text.c_str());
        return true;
    });

    p("mail", [=](gui::Item &) -> bool {
        LOG_INFO("SMS to: %s", text.c_str());
        return true;
    });

    p("cross", [=](gui::Item &) -> bool {
        LOG_INFO("Add contact: %s", text.c_str());
        return true;
    });

    box->resizeItems();
    return box;
}

std::list<gui::Item *> smsWindowOptions(app::ApplicationMessages *app, const SMSRecord *record)
{
    // TODO use record...
    // we oy - this list needs to be buildable with items too :(
    auto contact = ContactRecord();
    return {
        gui::newOptionLabel(callOption(app, contact, true)), gui::newOptionLabel(contactDetails(app, contact)),

        // TODO
        gui::newOptionLabel({UTF8(" <STUB> ") + UTF8(utils::localize.get("sms_forvard_message")), [=](gui::Item &item) { return false; }}),
        gui::newOptionLabel({UTF8(" <STUB> ") + UTF8(utils::localize.get("sms_copy")), [=](gui::Item &item) { return false; }}),
        gui::newOptionLabel({UTF8(" <STUB> ") + UTF8(utils::localize.get("sms_delete_message")), [=](gui::Item &item) { return false; }}),
        //        // ok - this is not selectable item...
        //        // tell num, message, call + - item with 3 elements...
        newCombo(app, "+123 123 123") // contact.numbers[0].numberE164)
    };
}
