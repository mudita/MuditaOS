// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
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
    // LEFT/RIGHT/UP/DOWN

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
        while (true) {
            auto textLine = gui::TextLine(drawCursor, w);

            if (textLine.length() == 0 && textLine.getLineEnd()) {
                debug_text_lines("cant show more text from this document");
                break;
            }

            if (lineYPosition + textLine.height() > h) { // no more space for next line
                debug_text_lines("no more space for next text_line: %d + %" PRIu32 " > %" PRIu32,
                                 lineYPosition,
                                 textLine.height(),
                                 h);
                break;
            }

            emplace(std::move(textLine));
            auto &line = last();

            line.setPosition(lineXPosition, lineYPosition);
            line.setParent(text);

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
        Length initHeight = text->getTextFormat().getFont()->info.line_height;

        while (true) {
            auto textLine =
                gui::TextLine(drawCursor, w, initHeight, underLine, UnderlineDrawMode::WholeLine, underLinePadding);

            if (textLine.height() > 0 && initHeight != textLine.height()) {
                initHeight = textLine.height();
            }

            if (lineYPosition + initHeight > h) {
                break;
            }

            if (lines.size() >= linesCount) {
                break;
            }

            emplace(std::move(textLine));
            auto &line = last();
            line.setPosition(lineXPosition, lineYPosition);
            line.setParent(text);

            lineYPosition += line.height();
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

} // namespace gui
