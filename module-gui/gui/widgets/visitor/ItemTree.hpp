// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ItemNode.hpp"

namespace gui
{
    /// General abstraction of `gui::Item`'s parent-children hierarchy with defined traverse order
    class ItemTree
    {
      public:
        [[nodiscard]] virtual auto hasNext() const noexcept -> bool = 0;
        /// Provides wrapper for next `gui::Item` object in traverse order. On call concrete implementations might
        /// change state of the tree by removing node being returned. Must be used in pair with `hasNode()`
        [[nodiscard]] virtual auto getNext() noexcept -> gui::ItemNode = 0;

        virtual ~ItemTree() = default;
    };
} // namespace gui
