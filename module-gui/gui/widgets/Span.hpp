#pragma once

#include "Item.hpp"

namespace gui
{
    /// span class - provides offset in selected axis
    class Span : public Item
    {
        Axis axis = Axis::X;

      public:
        Span(Axis axis, unsigned int size) : axis(axis)
        {
            setSize(size, axis);
            activeItem = false;
        }

        Span(Item *parent, Axis axis, unsigned int size) : Span(axis, size)
        {
            if (parent) {
                parent->addWidget(this);
            }
        }
    };
}; // namespace gui
