// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Item.hpp"

namespace gui
{
    class Item;

    class ItemNode
    {
        gui::Item &item;
        int level = 0;

      public:
        ItemNode(gui::Item &item, int level) : item{item}, level{level}
        {}

        [[nodiscard]] auto getItem() const noexcept -> gui::Item &
        {
            return item;
        }
        [[nodiscard]] auto getLevel() const noexcept -> int
        {
            return level;
        }
    };
} // namespace gui
