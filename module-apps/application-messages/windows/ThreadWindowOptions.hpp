// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationMessages.hpp"

#include <Interface/ThreadRecord.hpp>

namespace gui
{
    class Option;
}

std::list<gui::Option> threadWindowOptions(app::ApplicationMessages *appl, const ThreadRecord *record);
