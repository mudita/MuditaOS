#include "TextFixedSize.hpp"

namespace gui
{
    TextFixedSize::TextFixedSize(Item *parent, Position x, Position y, Length w, Length h, unsigned int linesCount)
        : Text(parent, x, y, w, h)
    {
        this->linesCount = linesCount;
        setEditMode(EditMode::EDIT);
        //
        //        inputText->setMinimumSize(phonebookStyle::inputLineWithLabelItem::w,phonebookStyle::inputLineWithLabelItem::input_text_h);
        //        inputText->setMaximumSize(phonebookStyle::inputLineWithLabelItem::w,phonebookStyle::inputLineWithLabelItem::input_text_h);
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

        uint32_t w = sizeMinusPadding(Axis::X, Area::Max);
        //        uint32_t h           = sizeMinusPadding(Axis::Y, Area::Max);
        auto line_y_position = padding.top;
        auto cursor          = 0;

        unsigned int currentLine = 0;

        auto line_x_position = padding.left;
        do {
            auto text_line = gui::TextLine(document.get(), cursor, w);
            cursor += text_line.length();

            if (text_line.length() == 0) {
                break;
            }
            //            if (line_y_position + text_line.height() > h) { // no more space for next line
            //                break;
            //            }

            // for each different text which fits in line, addWidget last - to not trigger callbacks to parent
            // on resizes while not needed, after detach there can be no `break` othervise there will be leak - hence
            // detach
            lines.emplace(std::move(text_line));
            auto &line = lines.last();
            line.setPosition(line_x_position, line_y_position);
            line.setParent(this);
            line.alignH(getAlignment(Axis::X), w);

            line_y_position += line.height();
            currentLine++;

        } while (currentLine != linesCount);

        lines.linesVAlign(sizeMinusPadding(Axis::Y, Area::Normal));
    }
} // namespace gui
