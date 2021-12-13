// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Item.hpp>

namespace gui
{
    /// class to disown Item temporary to ignore callback
    class ScopedParentDisown
    {
        Item *parent = nullptr;
        Item *item   = nullptr;

      public:
        explicit ScopedParentDisown(Item *it) : item(it)
        {
            if (item != nullptr) {
                parent = item->parent;
            }
        }

        ~ScopedParentDisown()
        {
            if (item != nullptr) {
                item->parent = parent;
            }
        }
    };
} // namespace gui
