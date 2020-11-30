// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TextLineCursor.hpp"
#include "Text.hpp"
#include "log/log.hpp"

#define debug_text_cursor(...)
// #define debug_text_cursor(...) LOG_DEBUG(__VA_ARGS__)

gui::TextLineCursor::TextLineCursor(gui::Text *parent, unsigned int pos, unsigned int block)
    : TextCursor(parent, pos, block)
{}

auto gui::TextLineCursor::moveCursor(gui::NavigationDirection direction) -> gui::TextCursor::Move
{
    debug_text_cursor("Before move cursor: screen pos: %d block: %d pos: %d %s",
                      pos_on_screen,
                      getBlockNr(),
                      BlockCursor::getPosition(),
                      atBegin() ? "at begin" : "middle");

    if (!checkDocument()) {
        return Move::Error;
    }

    auto [selectedLine, selectedLineCursorPos, selectedLineNr] = getSelectedLine();

    /// up - corner case
    if ((checkNpos() || (direction == NavigationDirection::UP && selectedLineNr == 0))) {
        return Move::Start;
    }

    /// down - corner case
    if ((checkNpos() || (direction == NavigationDirection::DOWN && selectedLineNr == text->lines->size() - 1))) {
        return Move::End;
    }

    if (direction == NavigationDirection::UP) {

        auto previousLine            = text->lines->getLine(selectedLineNr - 1);
        auto previousLineEndAddition = previousLine->getEnd() == TextBlock::End::Newline ? 1 : 0;

        auto previousLineMoveCount = previousLine->length() > selectedLineCursorPos
                                         ? previousLine->length() - previousLineEndAddition - selectedLineCursorPos
                                         : 0;

        auto moveCount = selectedLineCursorPos + previousLineEndAddition + previousLineMoveCount;

        TextCursor::moveCursor(NavigationDirection::LEFT, moveCount);

        debug_text_cursor("After move cursor: screen pos: %d block: %d pos: %d %s",
                          pos_on_screen,
                          getBlockNr(),
                          BlockCursor::getPosition(),
                          atBegin() ? "at begin" : "middle");

        return Move::Up;
    }

    if (direction == NavigationDirection::DOWN) {

        auto nextLine            = text->lines->getLine(selectedLineNr + 1);
        auto nextLineEndAddition = nextLine->getEnd() == TextBlock::End::Newline ? 1 : 0;

        auto nextLineMoveCount = nextLine->length() > selectedLineCursorPos ? selectedLineCursorPos
                                                                            : nextLine->length() - nextLineEndAddition;

        auto moveCount = (selectedLine->length() - selectedLineCursorPos) + nextLineMoveCount;

        TextCursor::moveCursor(NavigationDirection::RIGHT, moveCount);

        debug_text_cursor("After move cursor: screen pos: %d block: %d pos: %d %s",
                          pos_on_screen,
                          getBlockNr(),
                          BlockCursor::getPosition(),
                          atBegin() ? "at begin" : "middle");

        return Move::Down;
    }

    return TextCursor::moveCursor(direction);
}
