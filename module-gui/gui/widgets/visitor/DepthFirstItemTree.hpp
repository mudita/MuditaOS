// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ItemTree.hpp"
#include <stack>

namespace gui
{
    class DepthFirstItemTree : public ItemTree
    {
      public:
        /// Tree traverse modes. In `PreOrder` mode a parent precedes it's children. In `PostOrder` mode, children
        /// precedes it's parent.
        enum class TraverseMode
        {
            PreOrder,
            PostOrder
        };
        explicit DepthFirstItemTree(gui::Item &root, TraverseMode mode = TraverseMode::PreOrder);
        [[nodiscard]] auto hasNext() const noexcept -> bool override;
        [[nodiscard]] auto getNext() noexcept -> gui::ItemNode override;

      private:
        std::stack<gui::ItemNode> nodes;
        TraverseMode mode;

        void constructPostOrder(gui::Item *item, int level);
        [[nodiscard]] auto getNextInPreOrder() -> gui::ItemNode;
        [[nodiscard]] auto getNextInPostOrder() -> gui::ItemNode;
    };

} // namespace gui
