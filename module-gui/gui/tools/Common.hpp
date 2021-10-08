// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Item.hpp>

namespace gui
{

    /** Center \a child item in \a parent on \a axis
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

    /** Positon at \a child at \a parent bottom
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
