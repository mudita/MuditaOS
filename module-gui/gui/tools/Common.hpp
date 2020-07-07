#pragma once

#include <Item.hpp>

namespace gui
{

    /** Center \a child intem in \a paretn on \a axix
     */
    inline void center(gui::Item *parent, gui::Item *child, gui::Axis axis)
    {
        if (!parent || !child)
            return;
        auto center = (parent->area().size(axis) - child->area().size(axis)) / 2;
        if (center > 0) {
            child->setPosition(center, axis);
        }
    }

    /** Postion at \a child at \a parent bottom
     */
    inline void bottom(gui::Item *parent, gui::Item *child, gui::Axis axis)
    {
        if (!parent || !child)
            return;
        auto bottom = (parent->area().size(axis) - child->area().size(axis));
        if (bottom > 0) {
            child->setPosition(bottom, axis);
        }
    }

} // namespace gui
