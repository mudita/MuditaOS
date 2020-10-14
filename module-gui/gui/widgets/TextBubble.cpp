#include "TextBubble.hpp"

namespace gui
{

    TextBubble::TextBubble(Item *parent, Position x, Position y, Length w, Length h) : Text(parent, x, y, w, h)
    {
        setEditMode(EditMode::SCROLL);
        setEdges(RectangleEdge::All);
    }

    void TextBubble::setYaps(RectangleYap yaps)
    {
        if (this->yaps != yaps) {
            this->yaps = yaps;
            drawLines();
        }
    }

} // namespace gui
