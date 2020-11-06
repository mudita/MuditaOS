// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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
        auto checkNavigationBounds(TextLineCursor &cursor, InputEvent event) -> InputBound;
        auto checkAdditionBounds(TextLineCursor &cursor, InputEvent event) -> InputBound;
        auto checkRemovalBounds(TextLineCursor &cursor, InputEvent event) -> InputBound;
        void updateScrollPosition(NavigationDirection scroll, unsigned int lines_to_scroll = 1);

      protected:
        TextLine *getTextLine(uint32_t line);
    };

} // namespace gui
