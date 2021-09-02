// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Lines.hpp"
#include "TextLineCursor.hpp"
#include "Text.hpp"
#include "RawFont.hpp"

#if DEBUG_GUI_TEXT_LINES == 1
#define debug_text_lines(...) LOG_DEBUG(__VA_ARGS__)
#else
#define debug_text_lines(...)
#endif

namespace gui
{

    void Lines::addToInvisibleLines(TextLine line)
    {
        line.setVisible(false);
        emplace(std::move(line));
    }

    auto Lines::linesVAlign(Length parentSize) -> void
    {
        for (auto &line : lines) {
            line.alignV(text->getAlignment(Axis::Y), parentSize, linesHeight());
        }
    }

    auto Lines::linesHAlign(Length parentSize) -> void
    {
        for (auto &line : lines) {
            line.alignH(text->getAlignment(Axis::X), parentSize);
        }
    }

    auto Lines::draw(BlockCursor &drawCursor, Length w, Length h, Position lineYPosition, Position lineXPosition)
        -> void
    {
        Position initialTopPadding = lineYPosition;

        while (true) {
            auto textLine = gui::TextLine(drawCursor, w);

            if (textLine.length() == 0 && textLine.getLineEnd()) {
                debug_text_lines("cant show more text from this document");
                stopCondition = LinesDrawStop::OutOfText;
                break;
            }

            if ((lineYPosition + textLine.height()) > (h + initialTopPadding)) { // no more space for next line
                debug_text_lines("no more space for next text_line: %d + %" PRIu32 " > %" PRIu32,
                                 lineYPosition,
                                 textLine.height(),
                                 h);
                stopCondition = LinesDrawStop::OutOfSpace;
                addToInvisibleLines(std::move(textLine));
                break;
            }

            emplace(std::move(textLine));
            auto &line = last();

            line.setParent(text);
            line.setPosition(lineXPosition, lineYPosition);

            lineYPosition += line.height();
        }
    }

    auto Lines::draw(BlockCursor &drawCursor,
                     Length w,
                     Length h,
                     Position lineYPosition,
                     Position lineXPosition,
                     unsigned int linesCount) -> void
    {
        Position initialTopPadding = lineYPosition;
        Length initHeight = text->getTextFormat().getFont()->info.line_height;

        while (true) {
            auto textLine = gui::TextLine(drawCursor, w, initHeight, underLineProperties);

            if ((textLine.height() > 0) && initHeight != textLine.height()) {
                initHeight = textLine.height();
            }

            if (!previousLinesStart.empty() && (textLine.length() == 0) && textLine.getLineEnd()) {
                stopCondition = LinesDrawStop::OutOfText;
                break;
            }

            if ((lineYPosition + initHeight) > (h + initialTopPadding)) {
                if ((textLine.length() == 0) && textLine.getLineEnd()) {
                    stopCondition = LinesDrawStop::OutOfText;
                    break;
                }
                else {
                    stopCondition = LinesDrawStop::OutOfSpace;
                    addToInvisibleLines(std::move(textLine));
                    break;
                }
            }

            if (lines.size() >= linesCount) {
                if ((textLine.length() == 0) && textLine.getLineEnd()) {
                    stopCondition = LinesDrawStop::OutOfText;
                    break;
                }
                else {
                    stopCondition = LinesDrawStop::OutOfSpace;
                    addToInvisibleLines(std::move(textLine));
                    break;
                }
            }

            emplace(std::move(textLine));
            auto &line = last();

            line.setParent(text);
            line.setPosition(lineXPosition, lineYPosition);

            lineYPosition += line.height() + linesSpacing;
        }
    }

    TextLine *Lines::getLine(unsigned int lineNr)
    {
        if (lines.empty() || lineNr >= lines.size()) {
            return nullptr;
        }

        auto it = std::next(lines.begin(), lineNr);
        return &*it;
    }

    auto Lines::addToPreviousLinesStartList(unsigned int lineStartBlockNumber, unsigned int lineStartBlockPosition)
        -> void
    {
        auto it = std::find_if(
            previousLinesStart.begin(),
            previousLinesStart.end(),
            [&lineStartBlockNumber, &lineStartBlockPosition](const std::tuple<unsigned int, unsigned int> &lineStart) {
                return std::get<0>(lineStart) == lineStartBlockNumber &&
                       std::get<1>(lineStart) == lineStartBlockPosition;
            });

        if (it == previousLinesStart.end()) {
            previousLinesStart.emplace_back(lineStartBlockNumber, lineStartBlockPosition);
        }
    }

} // namespace gui
