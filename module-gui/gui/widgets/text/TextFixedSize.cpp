
// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TextFixedSize.hpp"

namespace gui
{
    TextFixedSize::TextFixedSize() : TextFixedSize(nullptr, 0, 0, 0, 0)
    {}

    TextFixedSize::TextFixedSize(Item *parent, Position x, Position y, Length w, Length h) : Text(parent, x, y, w, h)
    {
        setEditMode(EditMode::Edit);
        drawUnderline(true);
    }

    void TextFixedSize::setLines(const unsigned int val)
    {
        if (linesCount != val) {
            linesCount = val;
            drawLines();
        }
    }

    void TextFixedSize::setLinesSpacing(unsigned int val)
    {
        lines->setLineSpacing(val);
        drawLines();
    }

    void TextFixedSize::setUnderlinePadding(Position val)
    {
        if (lines->getUnderLineProperties().underLinePadding != val) {
            lines->getUnderLineProperties().underLinePadding = val;
            drawLines();
        }
    }

    void TextFixedSize::setUnderlineThickness(unsigned int val)
    {
        if (lines->getUnderLineProperties().thickness != val) {
            lines->getUnderLineProperties().thickness = val;
            drawLines();
        }
    }

    void TextFixedSize::drawLines()
    {
        lines->erase();

        const auto [startDrawBlockNumber, startDrawBlockPosition] = lines->drawStartConditions;
        BlockCursor drawCursor(
            cursor->getDocument(), startDrawBlockPosition, startDrawBlockNumber, getTextFormat().getFont());

        lines->draw(drawCursor,
                    getSizeMinusPadding(Axis::X, Area::Normal) - getCursorDrawSpace(),
                    getSizeMinusPadding(Axis::Y, Area::Normal),
                    padding.top,
                    padding.left,
                    linesCount,
                    textType);

        lines->linesHAlign(getSizeMinusPadding(Axis::X, Area::Normal));
        lines->linesVAlign(getSizeMinusPadding(Axis::Y, Area::Normal));
    }
} // namespace gui
