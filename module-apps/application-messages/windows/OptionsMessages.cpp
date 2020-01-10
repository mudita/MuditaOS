#include "OptionsMessages.hpp"
#include "UiCommon.hpp"
#include "i18/i18.hpp"
#include "log/log.hpp"

#include <BoxLayout.hpp>

gui::HBox *newCombo(app::ApplicationMessages *app, UTF8 &text)
{
    auto box = new gui::HBox(nullptr, 0, 0, style::window_width, style::window::label::big_h);
    auto l = new gui::Label(box, 0, 0, 300, style::window::label::big_h);
    style::window::decorate(l);
    l->setText(text);
    auto phone = new gui::Image(box, 0, 0, 50, style::window::label::big_h, "phonebook_phone_ringing");
    phone->activatedCallback = [=](gui::Item &) -> bool { return true; };
    auto mail = new gui::Image(box, 0, 0, 50, style::window::label::big_h, "mail");
    mail->activatedCallback = [=](gui::Item &) -> bool { return true; };
    auto cross = new gui::Image(box, 0, 0, 50, style::window::label::big_h, "cross");
    cross->activatedCallback = [=](gui::Item &) -> bool { return true; };
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
        //        newCombo(app, contact.numbers[0].numberE164)
    };
}
