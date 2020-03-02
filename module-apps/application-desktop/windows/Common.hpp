#pragma once
#include <Item.hpp>

inline void center(gui::Item *parent, gui::Item *child, gui::Axis axis)
{
    if (!parent || !child)
        return;
    auto center = (parent->area().size(axis) - child->area().size(axis)) / 2;
    if (center > 0)
    {
        child->area().pos(axis) += center;
    }
}

inline void bottom(gui::Item *parent, gui::Item *child, gui::Axis axis)
{
    if (!parent || !child)
        return;
    auto bottom = (parent->area().size(axis) - child->area().size(axis));
    if (bottom > 0)
    {
        child->area().pos(axis) += bottom;
    }
}
