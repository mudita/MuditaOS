// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BellBaseLayout.hpp"
#include <ListItemWithDescription.hpp>

namespace style::bell_sidelist_item
{
    inline constexpr auto description_font = style::window::font::largelight;
} // namespace style::bell_sidelist_item

namespace gui
{
    class BellSideListItem : public ListItemWithDescription
    {
      public:
        BellBaseLayout *body = nullptr;

        explicit BellSideListItem(const std::string &description);
    };
} /* namespace gui */
