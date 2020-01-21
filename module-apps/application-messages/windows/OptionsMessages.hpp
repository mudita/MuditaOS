#pragma once

#include "../ApplicationMessages.hpp"
#include "Interface/SMSRecord.hpp"
#include "OptionWindow.hpp"

std::list<gui::Item *> smsWindowOptions(app::ApplicationMessages *app, const SMSRecord &record);
