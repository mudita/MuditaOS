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
        uint32_t lower_bound     = 0;
        uint32_t upper_bound     = max_lines_count;

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
            if (max_lines_count > lines.size()) {
                lines.emplace_back(std::move(line));
            }
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
        void updateBounds(Scroll scroll, uint32_t factor = 1);

      protected:
        auto processTextInput(TextLineCursor &cursor, const InputEvent &event) -> InputBound;
        auto processNavigation(TextLineCursor &cursor, const InputEvent &event) -> InputBound;

        auto canMove(TextLineCursor &cursor, Scroll dir) -> InputBound;
        InputBound processRemoval(TextLineCursor &cursor);
        TextLine *getTextLine(uint32_t line);
        InputBound processAdding(TextLineCursor &cursor, const InputEvent &event);
    };

} // namespace gui