// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ItemNode.hpp"

namespace gui
{
    class ItemTree
    {
      public:
        [[nodiscard]] virtual auto hasNext() const noexcept -> bool    = 0;
        [[nodiscard]] virtual auto getNext() noexcept -> gui::ItemNode = 0;

        virtual ~ItemTree() = default;
    };
} // namespace gui
