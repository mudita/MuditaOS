#include "TextBubble.hpp"

namespace gui
{

    TextBubble::TextBubble(Item *parent, Position x, Position y, Length w, Length h) : Text(parent, x, y, w, h)
    {
        setEditMode(EditMode::SCROLL);
        setEdges(RectangleEdgeFlags::GUI_RECT_ALL_EDGES);
    }

    void TextBubble::setYaps(RectangleYapFlags yaps)
    {
        Rect::setYaps(yaps);
    }

} // namespace gui
