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
            if (parent) {
                parent->addWidget(this);
            }
        }
    };
}; // namespace gui
