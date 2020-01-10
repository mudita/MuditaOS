#pragma once

#include "../ApplicationMessages.hpp"
#include <OptionWindow.hpp>
#include <service-db/api/DBServiceAPI.hpp>

gui::Option callOption(app::ApplicationMessages *app, ContactRecord &contact, bool active);
gui::Option contactDetails(app::ApplicationMessages *app, ContactRecord &contact);
