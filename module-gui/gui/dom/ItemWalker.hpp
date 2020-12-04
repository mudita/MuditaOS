// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

namespace gui
{
    class Item;

    class ItemWalker
    {
      public:
        virtual void traverse(gui::Item &) = 0;
        virtual ~ItemWalker()              = default;
    };
} // namespace gui
