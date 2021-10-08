// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Item.hpp"

namespace gui
{
    /// span class - provides offset in selected axis
    class Span : public Item
    {

      public:
        Span(Axis axis, unsigned int size)
        {
            setSize(size, axis);
            activeItem = false;
            type       = ItemType::SPAN_ITEM;
        }

        Span(Item *parent, Axis axis, unsigned int size) : Span(axis, size)
        {
            if (parent != nullptr) {
                parent->addWidget(this);
            }
        }
    };
}; // namespace gui
