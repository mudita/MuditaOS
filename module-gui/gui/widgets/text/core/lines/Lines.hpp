// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "TextLine.hpp"
#include <text/TextConstants.hpp>

namespace gui
{
    using TextLineStartContition = std::tuple<unsigned int, unsigned int>;

    class Lines
    {
      private:
        Text *text = nullptr;
        std::list<TextLine> lines;
        UnderLineProperties underLineProperties;
        unsigned int linesSpacing = 0;

        void addToInvisibleLines(TextLine line);

        auto drawMultiLine(BlockCursor &drawCursor, Length w, Length h, Position lineYPosition, Position lineXPosition)
            -> void;
        auto drawMultiLine(BlockCursor &drawCursor,
                           Length w,
                           Length h,
                           Position lineYPosition,
                           Position lineXPosition,
                           unsigned int linesCount) -> void;
        auto drawSingleLine(BlockCursor &drawCursor, Length w, Length h, Position lineYPosition, Position lineXPosition)
            -> void;

      public:
        explicit Lines(Text *text) : text(text)
        {}

        ~Lines() = default;

        LinesDrawStop stopCondition                = LinesDrawStop::None;
        TextLineStartContition drawStartConditions = {0, 0};
        std::list<TextLineStartContition> previousLinesStart;

        void reset()
        {
            erase();

            stopCondition = LinesDrawStop::None;
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

        Length calculateLineSpacingAddition() const noexcept
        {
            return empty() ? 0 : (size() - 1) * linesSpacing;
        }

        Length calculateInitialCursorPosition(Length initHeight) const noexcept
        {
            return (empty() ? initHeight : initHeight * size()) + calculateLineSpacingAddition() +
                   underLineProperties.underLinePadding;
        }

        [[nodiscard]] auto linesHeight() const noexcept
        {
            return calculateLineSpacingAddition() +
                   std::accumulate(lines.begin(), lines.end(), 0U, [](const auto sum, const auto &line) {
                       return line.isVisible() ? sum + line.height() : sum;
                   });
        }

        [[nodiscard]] auto linesLength() const noexcept
        {
            return std::accumulate(lines.begin(), lines.end(), 0U, [](const auto sum, const auto &line) {
                return line.isVisible() ? sum + line.length() : sum;
            });
        }

        auto setUnderLineProperties(UnderLineProperties val) -> void
        {
            underLineProperties = val;
        }

        [[nodiscard]] UnderLineProperties &getUnderLineProperties() noexcept
        {
            return underLineProperties;
        }

        auto setLineSpacing(unsigned int val) -> void
        {
            linesSpacing = val;
        }

        auto draw(BlockCursor &drawCursor,
                  Length w,
                  Length h,
                  Position lineYPosition,
                  Position lineXPosition,
                  TextType drawType) -> void;
        auto draw(BlockCursor &drawCursor,
                  Length w,
                  Length h,
                  Position lineYPosition,
                  Position lineXPosition,
                  unsigned int linesCount,
                  TextType drawType) -> void;

        auto linesHAlign(Length parentSize) -> void;
        auto linesVAlign(Length parentSize) -> void;

        auto addToPreviousLinesStartList(unsigned int lineStartBlockNumber, unsigned int lineStartBlockPosition)
            -> void;

        TextLine *getLine(unsigned int lineNr = 0);
    };

} // namespace gui
