#include "ListItem.hpp"
#include <log/log.hpp>
#include <Style.hpp>

namespace gui
{

    ListItem::ListItem()
    {
        setRadius(0);
        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM | RectangleEdgeFlags::GUI_RECT_EDGE_TOP);

        setPenFocusWidth(style::window::default_border_focucs_w);
        setPenWidth(style::window::default_border_no_focus_w);
    }

    ListItem::~ListItem()
    {}

} /* namespace gui */
