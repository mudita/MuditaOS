// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "visitor/GuiVisitor.hpp"
#include "visitor/ItemWalker.hpp"

#include <memory>

namespace gui
{
    class Item;

    class ItemDomCreator
    {
        std::unique_ptr<ItemWalker> walker = nullptr;

      public:
        enum class DumpType
        {
            JsonAtTraverseEnd
        };

        explicit ItemDomCreator(DumpType type);
        [[nodiscard]] auto getWalker() noexcept -> std::unique_ptr<ItemWalker> &;
    };

} // namespace gui
