// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TextLineCursor.hpp"
#include <Text.hpp>
#include <log/log.hpp>

#define debug_text_cursor(...)
// #define debug_text_cursor(...) LOG_DEBUG(__VA_ARGS__)

namespace gui
{
    TextLineCursor::TextLineCursor(gui::Text *parent, unsigned int pos, unsigned int block)
        : TextCursor(parent, pos, block)
    {}

    auto TextLineCursor::checkNextLineDocumentEnd(unsigned int selectedLineNumber) -> bool
    {
        auto nextLine     = text->lines->getLine(selectedLineNumber + 1);
        auto selectedLine = text->lines->getLine(selectedLineNumber);

        return nextLine->length() == 0 && selectedLine->getEnd() != TextBlock::End::Newline;
    }

    void TextLineCursor::handleDownNavigation(unsigned int selectedLineNumber, unsigned int selectedLineCursorPosition)
    {
        auto selectedLine        = text->lines->getLine(selectedLineNumber);
        auto nextLine            = text->lines->getLine(selectedLineNumber + 1);
        auto nextLineEndAddition = nextLine->getEnd() == TextBlock::End::Newline ? 1 : 0;

        auto noNewAtBeginningAddition =
            selectedLine->getEnd() == TextBlock::End::None && selectedLineCursorPosition == 0 ? 1 : 0;

        auto nextLineMoveCount = nextLine->length() > selectedLineCursorPosition
                                     ? selectedLineCursorPosition
                                     : nextLine->length() - nextLineEndAddition;

        // Next line will be displayed as part of Right move action on right lines border
        auto moveCount =
            (selectedLine->length() - selectedLineCursorPosition) + nextLineMoveCount + noNewAtBeginningAddition;

        moveCursor(NavigationDirection::RIGHT, moveCount);
    }

    void TextLineCursor::handleUpNavigation(unsigned int selectedLineNumber, unsigned int selectedLineCursorPosition)
    {
        auto previousLine            = text->lines->getLine(selectedLineNumber - 1);
        auto previousLineEndAddition = previousLine->getEnd() == TextBlock::End::Newline ? 1 : 0;

        auto previousLineMoveCount = previousLine->length() > selectedLineCursorPosition
                                         ? previousLine->length() - previousLineEndAddition - selectedLineCursorPosition
                                         : 0;

        auto newLineAtBeginningSubtraction =
            (previousLine->getEnd() == TextBlock::End::Newline) && (selectedLineCursorPosition == 0) &&
                    (selectedLineNumber >= 2) &&
                    (text->lines->getLine(selectedLineNumber - 2)->getEnd() == TextBlock::End::None)
                ? 1
                : 0;

        auto moveCount = selectedLineCursorPosition + previousLineEndAddition + previousLineMoveCount -
                         newLineAtBeginningSubtraction;

        moveCursor(NavigationDirection::LEFT, moveCount);
    }

    auto TextLineCursor::displayPreviousLine() -> bool
    {
        if (!text->lines->previousLinesStart.empty()) {

            auto [lineStartBlockNumber, lineStartBlockPosition] = text->lines->previousLinesStart.back();
            text->lines->previousLinesStart.pop_back();
            text->lines->drawStartConditions = {lineStartBlockNumber, lineStartBlockPosition};

            text->drawLines();

            auto moveCount = text->lines->first().length();

            // update cursor position on screen so it points in same place after scroll
            onScreenPosition += moveCount;

            return true;
        }

        return false;
    }

    auto TextLineCursor::displayNextLine() -> bool
    {
        if (text->lines->stopCondition != LinesDrawStop::OutOfText) {

            text->lines->addToPreviousLinesStartList(text->lines->first().getLineStartBlockNumber(),
                                                     text->lines->first().getLineStartBlockPosition());

            auto moveCount = text->lines->first().length();

            text->lines->drawStartConditions = {text->lines->getLine(1)->getLineStartBlockNumber(),
                                                text->lines->getLine(1)->getLineStartBlockPosition()};

            text->drawLines();

            // update cursor position on screen so it points in same place after scroll
            onScreenPosition -= moveCount;

            return true;
        }

        return false;
    }

    auto TextLineCursor::moveCursor(gui::NavigationDirection direction) -> gui::TextCursor::Move
    {
        debug_text_cursor("Before move cursor: screen pos: %d block: %d pos: %d %s",
                          onScreenPosition,
                          getBlockNumber(),
                          BlockCursor::getPosition(),
                          atBegin() ? "at begin" : "middle");

        if (!checkDocument()) {
            return Move::Error;
        }

        auto [selectedLine, selectedLineCursorPosition, selectedLineNumber] = getSelectedLine();

        // last line is sometimes not visible (sometimes empty) and added only for scrolling purposes.
        unsigned int lastVisibleLineNumber = text->lines->countVisible() - 1;

        debug_text_cursor(
            "Selected line cursor pos: %d, selected line nr: %d", selectedLineCursorPosition, selectedLineNumber);

        // No lines draw yet to navigate top bottom -> check left/right text navigation
        if (selectedLine == nullptr) {
            return TextCursor::moveCursor(direction);
        }

        /// up - corner case
        if ((checkNpos() || (direction == NavigationDirection::UP && selectedLineNumber == 0))) {

            if (!text->lines->previousLinesStart.empty()) {

                displayPreviousLine();

                return moveCursor(NavigationDirection::UP);
            }
            else {
                return TextCursor::Move::Start;
            }
        }

        /// down - corner case
        if ((checkNpos() || (direction == NavigationDirection::DOWN && selectedLineNumber == lastVisibleLineNumber))) {

            if (text->lines->stopCondition != LinesDrawStop::OutOfText) {

                if (checkNextLineDocumentEnd(selectedLineNumber)) {
                    return TextCursor::Move::End;
                }

                handleDownNavigation(selectedLineNumber, selectedLineCursorPosition);

                return TextCursor::Move::Down;
            }
            else {
                return TextCursor::Move::End;
            }
        }

        /// left - corner case
        if ((checkNpos() || (direction == NavigationDirection::LEFT && selectedLineNumber == 0))) {

            if (!text->lines->previousLinesStart.empty() &&
                (text->lines->first().getLineStartBlockNumber() == getBlockNumber() &&
                 text->lines->first().getLineStartBlockPosition() == BlockCursor::getPosition())) {

                displayPreviousLine();

                return TextCursor::moveCursor(NavigationDirection::LEFT);
            }
            else {
                return TextCursor::moveCursor(direction);
            }
        }

        /// right - corner case
        if ((checkNpos() || (direction == NavigationDirection::RIGHT && selectedLineNumber >= lastVisibleLineNumber))) {

            auto onLastLine = selectedLineNumber == lastVisibleLineNumber;
            auto onLastLineLastCharacter =
                onLastLine &&
                (selectedLineCursorPosition ==
                 (text->lines->getLine(lastVisibleLineNumber)->length() -
                  (text->lines->getLine(lastVisibleLineNumber)->getEnd() == TextBlock::End::Newline ? 1 : 0)));

            if (text->lines->stopCondition != LinesDrawStop::OutOfText &&
                (onLastLineLastCharacter || (selectedLineNumber > lastVisibleLineNumber))) {

                if (onLastLine && checkNextLineDocumentEnd(selectedLineNumber)) {
                    return TextCursor::Move::End;
                }

                auto ret = TextCursor::moveCursor(NavigationDirection::RIGHT);

                displayNextLine();

                return ret;
            }
            else {
                return TextCursor::moveCursor(direction);
            }
        }

        if (direction == NavigationDirection::UP) {

            handleUpNavigation(selectedLineNumber, selectedLineCursorPosition);

            debug_text_cursor("After move cursor: screen pos: %d block: %d pos: %d %s",
                              onScreenPosition,
                              getBlockNumber(),
                              BlockCursor::getPosition(),
                              atBegin() ? "at begin" : "middle");

            return Move::Up;
        }

        if (direction == NavigationDirection::DOWN) {

            if (checkNextLineDocumentEnd(selectedLineNumber)) {
                return TextCursor::Move::End;
            }

            handleDownNavigation(selectedLineNumber, selectedLineCursorPosition);

            debug_text_cursor("After move cursor: screen pos: %d block: %d pos: %d %s",
                              onScreenPosition,
                              getBlockNumber(),
                              BlockCursor::getPosition(),
                              atBegin() ? "at begin" : "middle");

            return Move::Down;
        }

        return TextCursor::moveCursor(direction);
    }

    TextCursor::Move TextLineCursor::moveCursor(NavigationDirection direction, unsigned int n)
    {
        auto ret = TextCursor::Move::Start;

        for (unsigned int i = 0; i < n; i++) {
            ret = TextLineCursor::moveCursor(direction);

            if (ret == Move::Start || ret == Move::End || ret == Move::Error) {
                break;
            }
        }
        return ret;
    }

    bool TextLineCursor::removeChar()
    {
        auto linesSize = text->lines->size();

        auto ret = TextCursor::removeChar();

        // After sign removal check if lines count decreased - if so add previous line,
        if (ret && linesSize > text->lines->size()) {
            displayPreviousLine();
        }

        return ret;
    }

} // namespace gui
