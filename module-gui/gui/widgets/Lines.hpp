#pragma once

#include <memory>
#include <vector>

#include "Alignment.hpp"
#include "InputEvent.hpp"
#include "TextConstants.hpp"
#include "utf8/UTF8.hpp"

#include "InputMode.hpp"
#include "TextLine.hpp"
#include "Translator.hpp"

namespace gui
{
    class Text;
    class TextLineCursor;

    class Lines
    {
        Text *parent = nullptr;
        std::list<TextLine> lines;

        uint32_t max_lines_count = 4;
        uint32_t scroll_position = 0;

      public:
        Lines(Text *parent) : parent(parent)
        {}

        ~Lines() = default;

        void erase()
        {
            if (parent != nullptr) {
                for (auto &line : lines) {
                    line.erase();
                }
            }
            lines.clear();
        }

        void emplace(TextLine &&line)
        {
            lines.emplace_back(std::move(line));
        }

        const auto &get()
        {
            return lines;
        }

        auto &last()
        {
            return lines.back();
        }

        auto size()
        {
            return lines.size();
        }

        auto maxWidth()
        {
            unsigned int w = 0;
            // could be std::max_element
            for (auto &el : lines) {
                w = el.width() > w ? el.width() : w;
            }
            return w;
        }

        auto linesHeight()
        {
            unsigned int h = 0;
            for (auto &el : lines) {
                h += el.height();
            }
            return h;
        }

        void draw(TextCursor &cursor);

        void linesHAlign(Length parentSize);
        void linesVAlign(Length parentSize);
        auto checkBounds(TextLineCursor &cursor, InputEvent event) -> InputBound;
        void updateScrollPosition(NavigationDirection scroll, uint32_t lines_to_scroll = 1);

      protected:
        auto processTextInput(TextLineCursor &cursor, const InputEvent &event) -> InputBound;
        auto processNavigation(TextLineCursor &cursor, const InputEvent &event) -> InputBound;

        auto canMove(TextLineCursor &cursor, NavigationDirection dir) -> InputBound;
        InputBound processRemoval(TextLineCursor &cursor);
        TextLine *getTextLine(uint32_t line);
        InputBound processAdding(TextLineCursor &cursor, const InputEvent &event);
    };

} // namespace gui