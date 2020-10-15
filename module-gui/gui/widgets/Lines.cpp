#include "Lines.hpp"
#include "TextLineCursor.hpp"
#include "Text.hpp"

namespace gui
{
    auto navigationToScroll(NavigationDirection dir)
    {
        switch (dir) {
        case NavigationDirection::UP:
            return Scroll::UP;
        case NavigationDirection::DOWN:
            return Scroll::DOWN;
        case NavigationDirection::LEFT:
            return Scroll::LEFT;
        case NavigationDirection::RIGHT:
            return Scroll::RIGHT;
        default:
            return Scroll::NONE;
        }
    }

    // LEFT/RIGHT/UP/DOWN
    auto Lines::processNavigation(TextLineCursor &cursor, const InputEvent &event) -> gui::InputBound
    {
        auto dir = inputToNavigation(event);
        return canMove(cursor, navigationToScroll(dir));
    }

    auto Lines::canMove(TextLineCursor &cursor, Scroll dir) -> gui::InputBound
    {
        auto screen_bound = lower_bound;
        auto lines_bound  = lines.size();

        if (dir == Scroll::UP || dir == Scroll::RIGHT) {
            screen_bound = upper_bound;
            lines_bound  = 0;
        }

        if (cursor.getScreenLine() == lines_bound) {
            return InputBound::NO_DATA;
        }

        if (cursor.getScreenLine() == screen_bound) {
            return InputBound::HIT_BOUND;
        }

        return InputBound::CAN_MOVE;
    }

    gui::InputBound Lines::processAdding(TextLineCursor &cursor, const InputEvent &event)
    {
        auto format        = cursor->getFormat();
        uint32_t line      = cursor.getScreenLine();
        TextLine *textLine = getTextLine(line);

        if (textLine == nullptr || !cursor) {
            return InputBound::CAN_ADD;
        }

        auto code = parent->translator.handle(event.key, parent->mode != nullptr ? parent->mode->get() : "");

        auto bound = textLine->checkBounds(cursor, code, format);
        if (bound == InputBound::CANT_PROCESS && line == lower_bound) {
            return InputBound::CANT_PROCESS;
        }

        return InputBound::CAN_ADD;
    }

    auto Lines::processRemoval(TextLineCursor &cursor) -> gui::InputBound
    {
        if (lines.empty()) {
            return InputBound::CANT_PROCESS;
        }

        uint32_t line = cursor.getScreenLine();
        uint32_t pos  = cursor.getPosOnScreen();

        if (pos == 0) {
            if (line == upper_bound) {
                return InputBound::HIT_BOUND;
            }
            if (line == 0) {
                return InputBound::CANT_PROCESS;
            }
        }

        return InputBound::CAN_REMOVE;
    }

    auto Lines::processTextInput(TextLineCursor &cursor, const InputEvent &event) -> gui::InputBound
    {
        if (!parent->isMode(EditMode::EDIT)) {
            return gui::InputBound::CANT_PROCESS;
        }

        if (event.isShortPress() && event.is(KeyCode::KEY_PND)) {
            return processRemoval(cursor);
        }
        else {
            return processAdding(cursor, event);
        }
    }

    void Lines::updateBounds(Scroll scroll, uint32_t factor)
    {
        lower_bound += (static_cast<int>(scroll) * factor);
        upper_bound += (static_cast<int>(scroll) * factor);
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

    auto Lines::checkBounds(TextLineCursor &cursor, InputEvent event) -> InputBound
    {
        auto bound = processNavigation(cursor, event);

        if (bound & InputBound::CANT_PROCESS) {
            bound = processTextInput(cursor, event);
        }

        return bound;
    }

    void Lines::draw(TextCursor &cursor)
    {
        parent->drawLines();
    }

    TextLine *Lines::getTextLine(uint32_t line)
    {
        if (lines.empty() || line > lines.size()) {
            return nullptr;
        }

        auto it = std::next(lines.begin(), line);
        return &*it;
    } // namespace gui

} // namespace gui