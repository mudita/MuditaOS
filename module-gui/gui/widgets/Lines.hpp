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
    class Lines
    {
        Text *text = nullptr;
        std::list<TextLine> lines;

        bool underLine            = false;
        Position underLinePadding = 0;

      public:
        Lines(Text *text) : text(text)
        {}

        ~Lines() = default;

        void erase()
        {
            if (text != nullptr) {
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

        [[nodiscard]] auto empty() const noexcept -> bool
        {
            return lines.empty();
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

        auto setUnderLine(bool val) -> void
        {
            underLine = val;
        }

        auto getUnderLine() -> bool
        {
            return underLine;
        }

        auto setUnderLinePadding(Position val) -> void
        {
            underLinePadding = val;
        }

        auto getUnderLinePadding() -> Position
        {
            return underLinePadding;
        }

        auto draw(BlockCursor &drawCursor, Length w, Length h, Position lineYPosition, Position lineXPosition) -> void;
        auto draw(BlockCursor &drawCursor,
                  Length w,
                  Length h,
                  Position lineYPosition,
                  Position lineXPosition,
                  unsigned int linesCount) -> void;

        auto linesHAlign(Length parentSize) -> void;
        auto linesVAlign(Length parentSize) -> void;

        TextLine *getLine(unsigned int lineNr);
    };

} // namespace gui
