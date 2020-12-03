// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <vector>
#include <numeric>

#include "Alignment.hpp"
#include "InputEvent.hpp"
#include "TextConstants.hpp"
#include "utf8/UTF8.hpp"

#include "InputMode.hpp"
#include "TextLine.hpp"
#include "Translator.hpp"

namespace gui
{
    using TextLineStartContition = std::tuple<unsigned int, unsigned int>;

    class Lines
    {
        Text *text = nullptr;
        std::list<TextLine> lines;

        bool underLine            = false;
        Position underLinePadding = 0;

        void addToInvisibleLines(TextLine line);

      public:
        explicit Lines(Text *text) : text(text)
        {}

        ~Lines() = default;

        LinesDrawStop stopCondition                = LinesDrawStop::NONE;
        TextLineStartContition drawStartConditions = {0, 0};
        std::list<TextLineStartContition> previousLinesStart;

        void reset()
        {
            erase();

            stopCondition = LinesDrawStop::NONE;
            previousLinesStart.clear();
            drawStartConditions = {0, 0};
        }

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

        [[nodiscard]] auto &get()
        {
            return lines;
        }

        [[nodiscard]] auto &last()
        {
            return lines.back();
        }

        [[nodiscard]] auto &first()
        {
            return lines.front();
        }

        [[nodiscard]] auto size() const noexcept
        {
            return lines.size();
        }

        [[nodiscard]] auto countVisible() const noexcept
        {
            return std::count_if(lines.begin(), lines.end(), [](const auto &line) { return line.isVisible(); });
        }

        [[nodiscard]] auto empty() const noexcept
        {
            return lines.empty();
        }

        [[nodiscard]] auto maxWidth() const noexcept
        {
            Length w = 0;
            // could be std::max_element
            for (auto &el : lines) {
                if (el.isVisible()) {
                    w = std::max(el.width(), w);
                }
            }
            return w;
        }

        [[nodiscard]] auto linesHeight() const noexcept
        {
            return std::accumulate(lines.begin(), lines.end(), 0U, [](const auto sum, const auto &line) {
                return line.isVisible() ? sum + line.height() : sum;
            });
        }

        [[nodiscard]] auto linesLength() const noexcept
        {
            return std::accumulate(lines.begin(), lines.end(), 0U, [](const auto sum, const auto &line) {
                return line.isVisible() ? sum + line.length() : sum;
            });
        }

        auto setUnderLine(bool val) -> void
        {
            underLine = val;
        }

        [[nodiscard]] auto getUnderLine() const noexcept
        {
            return underLine;
        }

        auto setUnderLinePadding(Position val) -> void
        {
            underLinePadding = val;
        }

        [[nodiscard]] auto getUnderLinePadding() const noexcept
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

        auto addToPreviousLinesStartList(unsigned int lineStartBlockNumber, unsigned int lineStartBlockPosition)
            -> void;

        TextLine *getLine(unsigned int lineNr);
    };

} // namespace gui
