// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Tethering.hpp"

#include <log/log.hpp>
#include <i18n/i18n.hpp>

#include "Item.hpp"
#include "Style.hpp"

namespace gui::status_bar
{
    constexpr auto tethering_status = "tethering_status";

    Tethering::Tethering(Item *parent, std::uint32_t x, std::uint32_t y) : StatusBarWidgetBase(parent, x, y, 0, 0)
    {
        set(tethering_status, style::status_bar::imageTypeSpecifier);
    }
} // namespace gui::status_bar
