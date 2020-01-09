#pragma once

#include "../ApplicationMessages.hpp"
#include "Interface/SMSRecord.hpp"
#include "OptionWindow.hpp"

namespace gui
{
    namespace name
    {
        namespace window
        {
            inline const std::string messages_options = "options_mess";
        };
    }; // namespace name
};     // namespace gui

std::list<gui::Item *> smsWindowOptions(app::ApplicationMessages *app, const SMSRecord &record);
