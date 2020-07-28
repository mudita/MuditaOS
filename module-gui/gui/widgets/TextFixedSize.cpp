#include <module-gui/gui/core/RawFont.hpp>
#include "TextFixedSize.hpp"

namespace gui
{
    TextFixedSize::TextFixedSize(Item *parent, Position x, Position y, Length w, Length h, unsigned int linesCount)
        : Text(parent, x, y, w, h)
    {
        this->linesCount = linesCount;
        setEditMode(EditMode::EDIT);
    }

    void TextFixedSize::setLines(const unsigned int val)
    {
        linesCount = val;
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

        unsigned int currentLine = 1;
        unsigned int lineHeight  = font->info.line_height + 4;

        auto line_x_position = padding.left;
        do {
            auto text_line = gui::TextLine(document.get(), cursor, w, lineHeight);
            cursor += text_line.length();

            //            if (text_line.height() > 0 && lineHeight != text_line.height()) {
            //                lineHeight = text_line.height();
            //            }

            if (line_y_position + lineHeight > h) { // no more space for next line
                break;
            }

            // for each different text which fits in line, addWidget last - to not trigger callbacks to parent
            // on resizes while not needed, after detach there can be no `break` othervise there will be leak - hence
            // detach

            LOG_INFO("Co z tą pozycja %d", line_y_position);

            lines.emplace(std::move(text_line));
            auto &line = lines.last();
            line.setPosition(line_x_position, line_y_position);
            line.setParent(this);
            line.alignH(getAlignment(Axis::X), w);

            LOG_INFO("Ile mamy miejsca %d", h);
            LOG_INFO("Jaka wyskosc lini %d", text_line.height());
            LOG_INFO("Ile mamy lini %d", currentLine);

            line_y_position += lineHeight;

            LOG_INFO("debug text drawing: \n start cursor: %d line length: %d end cursor %d : document length "
                     "%d \n x: %d, y: %d \n%s",
                     cursor - lines.last().length(),
                     lines.last().length(),
                     cursor,
                     document->getText().length(),
                     line_x_position,
                     line_y_position,
                     [&]() -> std::string {
                         std::string text = document->getText();
                         return std::string(text.begin() + cursor - lines.last().length(), text.begin() + cursor);
                     }()
                                  .c_str());

            currentLine++;

        } while (true);

        // Vertical Align disabled for that TextWidget - discuss how to handle it
        lines.linesVAlign(sizeMinusPadding(Axis::Y, Area::Normal));
    }

    bool TextFixedSize::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        Text::onDimensionChanged(oldDim, newDim);
        drawLines();
        return true;
    }
} // namespace gui
