#pragma once

#include "../ApplicationPhonebook.hpp"
#include "Interface/ContactRecord.hpp"
#include "OptionWindow.hpp"

std::list<gui::Option> contactOptions(app::ApplicationPhonebook *app, const uint32_t contactId);
