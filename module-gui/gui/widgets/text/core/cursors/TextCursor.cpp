// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TextCursor.hpp"
#include <Text.hpp>

#include <log/log.hpp>

#define debug_text_cursor(...)
// #define debug_text_cursor(...) LOG_DEBUG(__VA_ARGS__)

namespace gui
{
    const unsigned int TextCursor::defaultWidth = 2;

    TextCursor::TextCursor(gui::Text *parent, unsigned int pos, unsigned int block)
        : Rect(parent, 0, 0, defaultWidth, 1),
          BlockCursor(parent != nullptr ? parent->document.get() : nullptr,
                      pos,
                      block,
                      parent != nullptr ? parent->getTextFormat().getFont() : nullptr),
          text(parent)
    {
        setFilled(true);
        setVisible(false);
    }

    TextCursor::Move TextCursor::moveCursor(NavigationDirection direction, unsigned int n)
    {
        auto ret = TextCursor::Move::Start;

        for (unsigned int i = 0; i < n; i++) {
            ret = moveCursor(direction);

            if (ret == Move::Start || ret == Move::End || ret == Move::Error) {
                break;
            }
        }
        return ret;
    }

    TextCursor::Move TextCursor::moveCursor(NavigationDirection direction)
    {
        debug_text_cursor("Before move cursor: screen pos: %d block: %d pos: %d %s",
                          onScreenPosition,
                          getBlockNumber(),
                          BlockCursor::getPosition(),
                          atBegin() ? "at begin" : "middle");

        if (!checkDocument()) {
            return Move::Error;
        }

        /// left - corner case
        if ((checkNpos() || atBegin()) && (direction == NavigationDirection::LEFT)) {
            return Move::Start;
        }

        /// right - corner case
        if ((checkNpos() || atEnd()) && (direction == NavigationDirection::RIGHT)) {
            return Move::End;
        }

        auto nr = getBlockNumber();
        if (direction == NavigationDirection::LEFT) {
            operator--();

            if (onScreenPosition > 0) {
                --onScreenPosition;
            }

            if (nr != getBlockNumber() && checkCurrentBlockNoNewLine()) {
                operator--();
            }

            return Move::Left;
        }

        if (direction == NavigationDirection::RIGHT) {
            operator++();

            if (onScreenPosition < document->getText().length()) {
                ++onScreenPosition;
            }

            if (nr != getBlockNumber() && checkPreviousBlockNoNewLine()) {
                operator++();
            }

            return Move::Right;
        }

        debug_text_cursor("After move cursor: screen pos: %d block: %d pos: %d %s",
                          onScreenPosition,
                          getBlockNumber(),
                          BlockCursor::getPosition(),
                          atBegin() ? "at begin" : "middle");

        return Move::Error;
    }

    std::tuple<const TextLine *, unsigned int, unsigned int> TextCursor::getSelectedLine()
    {
        unsigned int offsetPosition = 0;
        unsigned int row            = 0;
        if (text == nullptr) {
            return {nullptr, text::npos, text::npos};
        }
        for (auto &line : text->lines->get()) {

            auto lineSize = line.length() - (line.getEnd() == TextBlock::End::Newline ? 1 : 0);

            if (offsetPosition + lineSize >= onScreenPosition) {
                auto column = onScreenPosition - offsetPosition;
                return {&line, column, row};
            }
            offsetPosition += line.length();
            ++row;
        }
        return {nullptr, text::npos, text::npos};
    }

    void TextCursor::updateView()
    {
        int32_t x = 0, y = 0;
        uint32_t w = defaultWidth, h = 0;
        if (text == nullptr) {
            setArea({x, y, w, h});
            return;
        }
        auto default_font = text->format.getFont();
        if (document->isEmpty() && default_font != nullptr) {
            h += default_font->info.line_height;
            x = getAxisAlignmentValue(Axis::X, w);
            y = getAxisAlignmentValue(Axis::Y, text->lines->calculateInitialCursorPosition(h));
        }
        else if (text != nullptr || text->lines->size() > 0) {
            auto [line, column, row] = getSelectedLine();
            if (line == nullptr || column == text::npos || row == text::npos) {
                setArea({x, y, w, h});
                return;
            }

            auto el = line->getElement(column);
            assert(el != nullptr);
            x = line->getX() + line->getWidthTo(column);
            y = el->getY();
            h = el->getHeight();
        }
        setArea({x, y, w, h});
    }

    auto TextCursor::setCursorStartPosition(CursorStartPosition val) -> void
    {
        cursorStartPosition = val;
    }

    void TextCursor::addChar(uint32_t utf_val)
    {
        BlockCursor::addChar(utf_val);

        // lines need to be drawn before cursor move in case we have scrolling
        text->drawLines();

        if (cursorStartPosition != CursorStartPosition::DocumentBegin) {
            moveCursor(NavigationDirection::RIGHT);
        }
    }

    TextCursor &TextCursor::operator<<(const UTF8 &textString)
    {
        for (unsigned int i = 0; i < textString.length(); ++i) {
            if (text->checkAdditionBounds(textString[i]) == AdditionBound::CanAddAll) {
                addChar(textString[i]);
            }
            else {
                break;
            }
        }
        return *this;
    }

    TextCursor &TextCursor::operator<<(const TextBlock &textBlock)
    {
        auto [addBoundResult, processedTextBlock] = text->checkAdditionBounds(textBlock);

        if (addBoundResult == AdditionBound::CanAddAll || addBoundResult == AdditionBound::CanAddPart) {

            auto len = processedTextBlock.length();
            BlockCursor::addTextBlock(std::move(processedTextBlock));

            // lines need to be drawn before cursor move in case we have scrolling
            text->drawLines();

            // +1 is for block barrier
            if (cursorStartPosition != CursorStartPosition::DocumentBegin) {
                moveCursor(NavigationDirection::RIGHT, len + 1);
            }
        }

        return *this;
    }

    bool TextCursor::removeChar()
    {
        moveCursor(NavigationDirection::LEFT);
        if (BlockCursor::removeChar()) {
            text->drawLines();
            return true;
        }
        return false;
    }

    auto TextCursor::getAbsolutePosition() const -> unsigned int
    {
        auto pos     = 0U;
        auto blockNo = 0U;
        for (const auto &block : document->getBlocks()) {
            if (blockNo == getBlockNumber()) {
                break;
            }
            pos += block.length();
            blockNo++;
        }
        return pos + BlockCursor::getPosition();
    }

    bool TextCursor::handleNextLine()
    {
        return false;
    }
    bool TextCursor::handlePreviousLine()
    {
        return false;
    }

} // namespace gui

const char *c_str(enum gui::TextCursor::Move what)
{
    switch (what) {
    case gui::TextCursor::Move::Start:
        return "Start";
    case gui::TextCursor::Move::End:
        return "End";
    case gui::TextCursor::Move::Up:
        return "Up";
    case gui::TextCursor::Move::Down:
        return "Down";
    case gui::TextCursor::Move::Left:
        return "Left";
    case gui::TextCursor::Move::Right:
        return "Right";
    case gui::TextCursor::Move::Error:
        return "Error";
    }
    return "";
}
