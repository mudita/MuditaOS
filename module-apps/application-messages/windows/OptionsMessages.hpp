#pragma once

#include "../ApplicationMessages.hpp"
#include "Interface/SMSRecord.hpp"
#include "OptionWindow.hpp"

std::list<gui::Item *> smsWindowOptions(app::ApplicationMessages *app, const SMSRecord &record);

/// @brief options for New Message Window
///
/// @param app - application pointer
/// @param requestingWindow -requesting window - intentionally return point from an "action"
/// @param text - text object to be eventually modified by options operations
///
/// @return list of options
std::list<gui::Item *> newMessageWindowOptions(app::ApplicationMessages *app,
                                               const std::string &requestingWindow,
                                               gui::Text *text);
