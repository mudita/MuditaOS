#include <module-gui/gui/core/RawFont.hpp>
#include "TextFixedSize.hpp"

namespace gui
{
    TextFixedSize::TextFixedSize(Item *parent, Position x, Position y, Length w, Length h) : Text(parent, x, y, w, h)
    {
        setEditMode(EditMode::EDIT);
        underline = true;
    }

    void TextFixedSize::setLines(const unsigned int val)
    {
        if (linesCount != val) {
            linesCount = val;
            drawLines();
        }
    }

    void TextFixedSize::setUnderlinePadding(Position val)
    {
        if (underlinePadding != val) {
            underlinePadding = val;
            drawLines();
        }
    }

    void TextFixedSize::drawLines()
    {

        lines.erase();

        auto sizeMinusPadding = [&](Axis axis, Area val) {
            auto size = area(val).size(axis);
            if (size <= padding.getSumInAxis(axis)) {
                size = 0;
            }
            else {
                size -= padding.getSumInAxis(axis);
            }
            return size;
        };

        uint32_t w           = sizeMinusPadding(Axis::X, Area::Normal);
        uint32_t h           = sizeMinusPadding(Axis::Y, Area::Normal);
        auto line_y_position = padding.top;
        auto cursor          = 0;

        unsigned int currentLine = 0;
        unsigned int lineHeight  = font->info.line_height + underlinePadding;

        auto line_x_position = padding.left;
        do {
            auto text_line = gui::TextLine(
                document.get(), cursor, w, lineHeight, underline, UnderlineDrawMode::WholeLine, underlinePadding);
            cursor += text_line.length();

            if (text_line.height() > 0 && lineHeight != text_line.height()) {
                lineHeight = text_line.height();
            }

            if (line_y_position + lineHeight > h) {
                break;
            }

            if (currentLine >= linesCount) {
                break;
            }

            lines.emplace(std::move(text_line));
            auto &line = lines.last();
            line.setPosition(line_x_position, line_y_position);
            line.setParent(this);

            line_y_position += lineHeight;

            currentLine++;

        } while (true);

        lines.linesHAlign(sizeMinusPadding(Axis::X, Area::Normal));
        lines.linesVAlign(sizeMinusPadding(Axis::Y, Area::Normal));
    }
} // namespace gui
