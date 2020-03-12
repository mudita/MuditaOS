#pragma once

#include "../ApplicationCallLog.hpp"
#include "Interface/CalllogRecord.hpp"
#include "OptionWindow.hpp"

std::list<gui::Option> calllogWindowOptions(app::ApplicationCallLog *appl, const CalllogRecord &record);
