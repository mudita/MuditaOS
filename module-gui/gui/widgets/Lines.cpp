// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Lines.hpp"
#include "TextLineCursor.hpp"
#include "Text.hpp"

namespace gui
{
    // LEFT/RIGHT/UP/DOWN
    auto Lines::checkNavigationBounds(TextLineCursor &cursor, InputEvent event) -> gui::InputBound
    {
        auto dir = inputToNavigation(event);
        if (dir == NavigationDirection::NONE) {
            return InputBound::UNDEFINED;
        }

        auto screen_bound = scroll_position + max_lines_count - 1;
        auto lines_bound  = lines.size() - 1;

        if (dir == NavigationDirection::UP || dir == NavigationDirection::LEFT) {
            screen_bound = 0;
            lines_bound  = 0;
        }

        if (dir == NavigationDirection::LEFT && cursor.getPosOnScreen() > 0) {
            return InputBound::CAN_MOVE;
        }

        unsigned int pos = cursor.BlockCursor::getPosition();
        auto textLine    = getTextLine(cursor.getScreenLine());

        if (textLine == nullptr) {
            return InputBound::NO_DATA;
        }

        size_t lineLength = textLine->length();

        if (dir == NavigationDirection::RIGHT && pos < lineLength) {
            return InputBound::CAN_MOVE;
        }

        if (cursor.getScreenLine() >= lines_bound) {
            return InputBound::NO_DATA;
        }

        if (cursor.getScreenLine() >= screen_bound) {
            return InputBound::HIT_BOUND;
        }

        return InputBound::CAN_MOVE;
    }

    auto Lines::checkAdditionBounds(TextLineCursor &cursor, InputEvent event) -> gui::InputBound
    {
        auto keymap = parent->mode != nullptr ? parent->mode->get() : "";
        auto code   = gui::Profiles::get(keymap).get(event.key.key_code, 0);

        if (code == KeyProfile::none_key && event.isShortPress()) {
            return InputBound::CANT_PROCESS;
        }

        auto format        = cursor->getFormat();
        uint32_t line      = cursor.getScreenLine();
        TextLine *textLine = getTextLine(line);

        if (textLine == nullptr || !cursor) {
            return InputBound::CAN_ADD;
        }

        auto bound = textLine->checkBounds(cursor, code, format);
        if (bound == InputBound::CANT_PROCESS && line == scroll_position) {
            return InputBound::CAN_ADD;
        }

        return InputBound::CAN_ADD;
    }

    auto Lines::checkRemovalBounds(TextLineCursor &cursor, InputEvent event) -> gui::InputBound
    {
        if (lines.empty()) {
            return InputBound::CANT_PROCESS;
        }

        uint32_t line = cursor.getScreenLine();
        uint32_t pos  = cursor.getPosOnScreen();

        if (pos == 0) {
            if (line == scroll_position + max_lines_count) {
                return InputBound::HIT_BOUND;
            }
            if (line == 0) {
                return InputBound::CANT_PROCESS;
            }
        }

        return InputBound::CAN_REMOVE;
    }

    void Lines::updateScrollPosition(NavigationDirection dir, unsigned int lines_to_scroll)
    {
        if (dir == NavigationDirection::UP) {
            scroll_position -= lines_to_scroll;
        }

        if (dir == NavigationDirection::DOWN) {
            scroll_position += lines_to_scroll;
        }
    }

    void Lines::linesVAlign(Length parentSize)
    {
        for (auto &line : lines) {
            line.alignV(parent->getAlignment(Axis::Y), parentSize, linesHeight());
        }
    }

    void Lines::linesHAlign(Length parentSize)
    {
        for (auto &line : lines) {
            line.alignH(parent->getAlignment(Axis::X), parentSize);
        }
    }

    void Lines::draw(TextCursor &cursor)
    {
        parent->drawLines();
    }

    TextLine *Lines::getTextLine(uint32_t line)
    {
        if (lines.empty() || line >= lines.size()) {
            return nullptr;
        }

        auto it = std::next(lines.begin(), line);
        return &*it;
    } // namespace gui

} // namespace gui