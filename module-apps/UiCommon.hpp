#pragma once

#include "application-messages/ApplicationMessages.hpp"
#include <OptionWindow.hpp>
#include <service-db/api/DBServiceAPI.hpp>

bool call(app::ApplicationMessages *app, const ContactRecord contact);
bool sms(app::ApplicationMessages *app, const ContactRecord contact);
// TODO use contact here
bool addContact(app::ApplicationMessages *app, const ContactRecord contact);
gui::Option callOption(app::ApplicationMessages *app, ContactRecord contact, bool active);
gui::Option contactDetails(app::ApplicationMessages *app, ContactRecord &contact);
