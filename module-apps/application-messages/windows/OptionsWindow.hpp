#pragma once

#include "../ApplicationMessages.hpp"
#include "Interface/ThreadRecord.hpp"
#include "OptionWindow.hpp"

namespace gui
{
    namespace name
    {
        namespace window
        {
            inline const std::string thread_options = "options";
        };
    }; // namespace name
};     // namespace gui

std::list<gui::Option> threadWindowOptions(app::ApplicationMessages *appl, const ThreadRecord *record);
