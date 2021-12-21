// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TextInLineCursor.hpp"
#include <Text.hpp>
#include <log/log.hpp>

#if DEBUG_GUI_TEXT_CURSOR == 1
#define debug_text_cursor(...) LOG_DEBUG(__VA_ARGS__)
#else
#define debug_text_cursor(...)
#endif

namespace gui
{
    TextInLineCursor::TextInLineCursor(gui::Text *parent, unsigned int pos, unsigned int block)
        : TextCursor(parent, pos, block)
    {}

    bool TextInLineCursor::checkInLineLeftMove()
    {
        return onScreenPosition == 0 && (BlockCursor::getBlockNumber() != 0 || BlockCursor::getPosition() != 0);
    }

    void TextInLineCursor::moveInLineLeft()
    {
        const auto [startDrawBlockNumber, startDrawBlockPosition] = text->lines->drawStartConditions;
        BlockCursor moveCursor(
            getDocument(), startDrawBlockPosition, startDrawBlockNumber, text->getTextFormat().getFont());
        --moveCursor;

        text->lines->drawStartConditions = {moveCursor.getBlockNumber(), moveCursor.getPosition()};
        text->drawLines();
    }

    bool TextInLineCursor::checkInLineRightMove()
    {
        return onScreenPosition >= text->lines->getLine()->length() &&
               (getAbsolutePosition() != text->getText().length());
    }

    void TextInLineCursor::moveInLineRight()
    {
        const auto [startDrawBlockNumber, startDrawBlockPosition] = text->lines->drawStartConditions;
        auto textFont                                             = text->getTextFormat().getFont();

        BlockCursor moveCursor(getDocument(), startDrawBlockPosition, startDrawBlockNumber, textFont);

        unsigned int moveWidth = 0;
        while (calculateMinWidthToMoveWithEllipsis(textFont) > moveWidth) {
            moveWidth += textFont->getCharPixelWidth(moveCursor.getText().front());
            ++moveCursor;
            --onScreenPosition;
        }

        text->lines->drawStartConditions = {moveCursor.getBlockNumber(), moveCursor.getPosition()};
        text->drawLines();
    }

    auto TextInLineCursor::moveCursor(gui::NavigationDirection direction) -> gui::TextCursor::Move
    {
        debug_text_cursor("Before move cursor: screen pos: %d block: %d pos: %d %s",
                          onScreenPosition,
                          getBlockNumber(),
                          BlockCursor::getPosition(),
                          atBegin() ? "at begin" : "middle");

        if (!checkDocument()) {
            return Move::Error;
        }

        if (direction == NavigationDirection::LEFT && checkInLineLeftMove()) {
            moveInLineLeft();
        }

        if (direction == NavigationDirection::RIGHT && checkInLineRightMove()) {
            moveInLineRight();
        }
        return TextCursor::moveCursor(direction);
    }

    TextCursor::Move TextInLineCursor::moveCursor(NavigationDirection direction, unsigned int n)
    {
        auto ret = TextCursor::Move::Start;

        for (unsigned int i = 0; i < n; i++) {
            ret = TextInLineCursor::moveCursor(direction);

            if (ret == Move::Start || ret == Move::End || ret == Move::Error) {
                break;
            }
        }
        return ret;
    }

    bool TextInLineCursor::removeChar()
    {
        auto ret = TextCursor::removeChar();

        if (ret && checkInLineLeftMove()) {
            moveInLineLeft();
            onScreenPosition++;
        }

        return ret;
    }

    unsigned int TextInLineCursor::calculateMinWidthToMoveWithEllipsis(RawFont *font)
    {
        if (text->getTextEllipsisType() == TextEllipsis::Left || text->getTextEllipsisType() == TextEllipsis::Both) {
            return std::max(font->getCharPixelWidth(getText().front()), font->getPixelWidth(text::ellipsis_signs));
        }
        else {
            return font->getCharPixelWidth(getText().front());
        }
    }

} // namespace gui
