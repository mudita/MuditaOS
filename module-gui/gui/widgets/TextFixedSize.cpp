
// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-gui/gui/core/RawFont.hpp>
#include "TextFixedSize.hpp"

namespace gui
{
    TextFixedSize::TextFixedSize(Item *parent, Position x, Position y, Length w, Length h) : Text(parent, x, y, w, h)
    {
        setEditMode(EditMode::EDIT);
        lines->setUnderLine(true);
    }

    void TextFixedSize::setLines(const unsigned int val)
    {
        if (linesCount != val) {
            linesCount = val;
            drawLines();
        }
    }

    void TextFixedSize::setUnderlinePadding(Position val)
    {
        if (lines->getUnderLinePadding() != val) {
            lines->setUnderLinePadding(val);
            drawLines();
        }
    }

    void TextFixedSize::drawLines()
    {
        lines->erase();

        BlockCursor drawCursor(cursor->getDocument(), 0, 0, getTextFormat().getFont());

        lines->draw(drawCursor,
                    getSizeMinusPadding(Axis::X, Area::Normal),
                    getSizeMinusPadding(Axis::Y, Area::Normal),
                    padding.top,
                    padding.left,
                    linesCount);

        lines->linesHAlign(getSizeMinusPadding(Axis::X, Area::Normal));
        lines->linesVAlign(getSizeMinusPadding(Axis::Y, Area::Normal));
    }
} // namespace gui
