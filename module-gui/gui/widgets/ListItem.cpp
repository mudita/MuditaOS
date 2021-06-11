// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ListItem.hpp"
#include <log.hpp>
#include <Style.hpp>

namespace gui
{

    ListItem::ListItem()
    {
        setRadius(0);
        setEdges(RectangleEdge::Bottom | RectangleEdge::Top);

        setPenFocusWidth(style::window::default_border_focus_w);
        setPenWidth(style::window::default_border_no_focus_w);

        type = ItemType::LIST_ITEM;
    }

    void ListItem::accept(GuiVisitor &visitor)
    {
        visitor.visit(*this);
    }

} /* namespace gui */
