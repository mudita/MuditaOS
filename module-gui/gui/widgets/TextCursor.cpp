#include "TextCursor.hpp"
#include "TextDocument.hpp"

namespace gui
{

    TextCursor::TextCursor(gui::Item *parent) : Rect(parent, 0, 0, 1, 1)
    {
        setFilled(true);
        setVisible(false);
    }

    [[nodiscard]] unsigned int TextCursor::getRow() const
    {
        return row;
    }

    [[nodiscard]] unsigned int TextCursor::getColumn() const
    {
        return column;
    }

    void TextCursor::setRow(unsigned int row)
    {
        this->row = row;
    }

    void TextCursor::setColumn(unsigned int column)
    {
        this->column = column;
    }

    void TextCursor::update(unsigned int row, unsigned int col)
    {
        this->row    = row;
        this->column = col;
    }

    void TextCursor::reset()
    {
        update(0, 0);
    }

    [[nodiscard]] auto TextCursor::atTextBegin() const -> bool
    {
        return row == 0 && column == 0;
    }

    bool TextCursor::move(const NavigationDirection &direction, std::unique_ptr<TextDocument> &document, TextType type)
    {

        auto it = document->getTextBlockByCursorRow(getRow());

        if (direction == NavigationDirection::LEFT) {
            // if we are standing on the beginning for the line move to previous line
            if (column == 0) {

                // if there is no previous line return false so window can switch focus to the item on the left.
                if (it == document->lines.begin()) {
                    return false;
                }

                // there is a previous line, check if cursor's row is greater than 0;
                column = (*std::prev(it, 1))->text.length();
                if (row > 0) {
                    --row;
                }
                else {
                    --document->firstLine;
                    // recalculateDrawParams();
                }
                return true;
            }
            // cursor's column is greater than 0
            else {
                --column;
                return true;
            }
        }
        else if (direction == NavigationDirection::RIGHT) {
            // if cursor is not at the end of current line simply move curosr
            if (column < (*it)->text.length()) {
                ++column;
                return true;
            }
            else {
                auto itNext = std::next(it, 1);
                // if this is not the last line increment row and set column to 0
                if (itNext != document->lines.end()) {
                    ++row;
                    column = 0;

                    // if increased row is out of visible are increment first line
                    if (row >= document->getVisibleRowCount()) {
                        document->firstLine++;
                        // recalculateDrawParams();
                        row = document->getVisibleRowCount() - 1;
                    }
                    return true;
                }
            }
        }
        else if (direction == NavigationDirection::DOWN) {

            // if this is a single line text widget there is no navigation down allowed
            if (type == TextType::SINGLE_LINE)
                return false;

            auto itNext = std::next(it, 1);

            if (itNext == document->lines.end()) {
                return false;
            }

            // increment line
            ++row;

            // check if column position is still valid
            if (column > (*itNext)->text.length()) {
                column = (*itNext)->text.length();
            }

            if (row >= document->getVisibleRowCount()) {
                document->firstLine++;
                // recalculateDrawParams();
                row = document->getVisibleRowCount() - 1;
            }
            return true;
        }
        else if (direction == NavigationDirection::UP) {

            // if this is a single line text widget there is no navigation up allowed
            if (type == TextType::SINGLE_LINE)
                return false;

            // if cursor is standing on the first line return false to allow focus change to previous widget
            if (it == document->lines.begin()) {
                return false;
            }

            auto itPrev = std::prev(it, 1);
            if (row == 0) {
                --document->firstLine;

                // recalculateDrawParams();
                return true;
            }
            else {
                --row;
            }

            // check if previous line is shorter than last one if so move cursor to the end of previous line
            if (column > (*itPrev)->text.length()) {
                column = (*itPrev)->text.length();
            }

            return true;
        }

        return false;
    }

} // namespace gui
