#pragma once

#include "../ApplicationMessages.hpp"
#include "Interface/ThreadRecord.hpp"
#include "OptionWindow.hpp"

std::list<gui::Option> threadWindowOptions(app::ApplicationMessages *appl, const ThreadRecord *record);
