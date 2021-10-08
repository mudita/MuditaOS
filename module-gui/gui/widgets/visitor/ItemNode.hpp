// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Item.hpp"

namespace gui
{
    class Item;

    /// General wrapper for a `gui::Item`'s parent-children hierarchy. Encapsulates a object's reference together with
    /// it's level in a hierarchy
    class ItemNode
    {
      public:
        static constexpr auto rootLevel = 0;

        explicit ItemNode(gui::Item &item, int level = rootLevel) : item{item}, level{level}
        {}

        [[nodiscard]] auto getItem() noexcept -> gui::Item &
        {
            return item;
        }

        [[nodiscard]] auto getLevel() const noexcept -> int
        {
            return level;
        }

      private:
        gui::Item &item;
        int level = rootLevel;
    };
} // namespace gui
