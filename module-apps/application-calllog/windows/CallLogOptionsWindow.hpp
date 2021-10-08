// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationCallLog.hpp"
#include "Interface/CalllogRecord.hpp"
#include "OptionWindow.hpp"

std::list<gui::Option> calllogWindowOptions(app::ApplicationCallLog *appl, const CalllogRecord &record);
