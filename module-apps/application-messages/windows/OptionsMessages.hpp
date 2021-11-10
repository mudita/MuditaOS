// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationMessages.hpp"

#include <OptionWindow.hpp>
#include <Interface/SMSRecord.hpp>
#include <Option.hpp>

namespace gui
{

    class SmsWindowOptions : public OptionWindow
    {
      public:
        explicit SmsWindowOptions(app::ApplicationCommon *app, std::string windowName);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

      private:
        enum class ValidCodes
        {
            Uninitialized,
            Invalid,
            Valid
        };

        SMSRecord record;
        ValidCodes recordValidCode{ValidCodes::Uninitialized};

        std::list<gui::Option> smsWindowOptions(app::ApplicationMessages *app, const SMSRecord &record);
        ValidCodes getRecordValid();
    };

} // namespace gui

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
