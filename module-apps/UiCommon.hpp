#pragma once

#include "Application.hpp"
#include <OptionWindow.hpp>
#include <service-db/api/DBServiceAPI.hpp>

namespace app
{
    bool call(Application *app, const ContactRecord &contact);
    bool sms(Application *app, const ContactRecord &contact);
    // TODO use contact here
    bool addContact(Application *app, const ContactRecord &contact);
    bool addContact(Application *app, const std::string &number);
    gui::Option callOption(Application *app, const ContactRecord &contact, bool active);
    gui::Option contactDetails(Application *app, const ContactRecord &contact);
} // namespace app
