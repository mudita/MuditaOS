// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TextBubble.hpp"

namespace gui
{

    TextBubble::TextBubble(Item *parent, Position x, Position y, Length w, Length h) : Text(parent, x, y, w, h)
    {
        setEditMode(EditMode::Scroll);
        setEdges(RectangleEdge::All);
        setCursorStartPosition(CursorStartPosition::DocumentBegin);
    }

    void TextBubble::setYaps(RectangleYap yaps)
    {
        if (this->yaps != yaps) {
            this->yaps = yaps;
            drawLines();
        }
    }

} // namespace gui
