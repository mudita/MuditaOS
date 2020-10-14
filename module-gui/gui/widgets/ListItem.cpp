#include "ListItem.hpp"
#include <log/log.hpp>
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
} /* namespace gui */
