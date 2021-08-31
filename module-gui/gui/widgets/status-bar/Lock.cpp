// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Lock.hpp"
#include "Style.hpp"

namespace gui::status_bar
{
    constexpr auto lock = "locked_status";

    Lock::Lock(Item *parent, uint32_t x, uint32_t y) : StatusBarWidgetBase(parent, x, y, 0, 0)
    {
        set(lock, style::status_bar::imageTypeSpecifier);
    }
}; // namespace gui::status_bar
