#pragma once

#include "../ApplicationMessages.hpp"
#include "Interface/ThreadRecord.hpp"

namespace gui
{
    class Option;
}

std::list<gui::Option> threadWindowOptions(app::ApplicationMessages *appl, const ThreadRecord *record);
