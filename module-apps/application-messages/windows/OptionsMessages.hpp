// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationMessages.hpp"

#include <Interface/SMSRecord.hpp>
#include <Option.hpp>

std::list<gui::Option> smsWindowOptions(app::ApplicationMessages *app, const SMSRecord &record);

/// @brief options for New Message Window
///
/// @param app - application pointer
/// @param requestingWindow -requesting window - intentionally return point from an "action"
/// @param text - text object to be eventually modified by options operations
///
/// @return list of options
std::list<gui::Option> newMessageWindowOptions(app::ApplicationMessages *app,
                                               const std::string &requestingWindow,
                                               gui::Text *text);
