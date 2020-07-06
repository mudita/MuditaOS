#include "TextCursor.hpp"
#include "Common.hpp"
#include "Text.hpp"
#include "TextBlockCursor.hpp"
#include "TextDocument.hpp"
#include "log/log.hpp"
#include "TextLine.hpp"
#include <cassert>

#define debug_text_cursor(...)
// #define debug_text_cursor(...) LOG_DEBUG(__VA_ARGS__)

namespace gui
{
    const unsigned int TextCursor::default_width = 2;

    TextCursor::TextCursor(gui::Text *parent, gui::TextDocument *document)
        : Rect(parent, 0, 0, default_width, 1), BlockCursor(document, 0, 0, parent != nullptr ? parent->font : nullptr),
          text(parent)
    {
        setFilled(true);
        setVisible(false);
    }

    TextCursor::Move TextCursor::move(NavigationDirection direction)
    {
        debug_text_cursor("cursor: screen pos: %d block: %d pos: %d %s",
                          pos_on_screen,
                          getBlockNr(),
                          BlockCursor::getPosition(),
                          atBegin() ? "at begin" : "middle");
        /// left & up - corner case
        if ((checkNpos() || atBegin()) &&
            (direction == NavigationDirection::LEFT || direction == NavigationDirection::UP)) {
            return Move::Start;
        }

        /// down & right - corner case
        if ((checkNpos() || atEnd()) &&
            (direction == NavigationDirection::RIGHT || direction == NavigationDirection::DOWN)) {
            return Move::End;
        }

        auto nr = getBlockNr();
        if (direction == NavigationDirection::LEFT) {
            operator--();
            if (nr == getBlockNr() || nr == text::npos) {
                --pos_on_screen;
            }
            return Move::InLine;
        }

        if (direction == NavigationDirection::RIGHT) {
            operator++();
            if (nr == getBlockNr() || nr == text::npos) {
                ++pos_on_screen;
            }
            return Move::InLine;
        }

        return Move::Error;
    }

    std::tuple<const TextLine *, unsigned int, unsigned int> TextCursor::getLine()
    {
        unsigned int offset_pos = 0;
        unsigned int row        = 0;
        if (text == nullptr) {
            return {nullptr, text::npos, text::npos};
        }
        for (auto &line : text->lines.get()) {
            if (offset_pos + line.length() >= pos_on_screen) {
                auto column = pos_on_screen - offset_pos;
                return {&line, column, row};
            }
            offset_pos += line.length();
            ++row;
        }
        return {nullptr, text::npos, text::npos};
    }

    void TextCursor::updateView()
    {
        int32_t x = 0, y = 0;
        uint32_t w = default_width, h = 0;
        if (text == nullptr) {
            setArea({x, y, w, h});
            return;
        }
        if (document->isEmpty() && text->font != nullptr) {
            h += text->font->info.line_height;
        }
        else if (text != nullptr || text->lines.size() > 0) {
            auto [line, column, row] = getLine();
            if (line == nullptr || column == text::npos || row == text::npos) {
                setArea({x, y, w, h});
                return;
            }
            auto el = line->getElement(column);
            assert(el != nullptr);
            x += line->getX() + line->getWidthTo(column);
            y += el->getY();
            h += el->getHeight();
        }
        setArea({x, y, w, h});
    }

    void TextCursor::addChar(uint32_t utf_val)
    {
        BlockCursor::addChar(utf_val);
        move(NavigationDirection::RIGHT);
    }

    TextCursor &TextCursor::operator<<(const UTF8 &text)
    {
        for (auto el : std::string(text)) {
            addChar(el);
        }
        return *this;
    }

    void TextCursor::removeChar()
    {
        move(NavigationDirection::LEFT);
        BlockCursor::removeChar();
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
    case gui::TextCursor::Move::InLine:
        return "InLine";
    case gui::TextCursor::Move::Error:
        return "Error";
    }
    return "";
}
