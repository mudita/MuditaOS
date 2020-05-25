#pragma once

#include "Rect.hpp"
#include "TextConstants.hpp"
#include <memory>

namespace gui
{

    class TextDocument;

    class TextCursor : public Rect
    {
      public:
        unsigned int row    = 0;
        unsigned int column = 0;

        TextCursor(gui::Item *parent) : Rect(parent, 0, 0, 1, 1)
        {
            setFilled(true);
            setVisible(false);
        }

        [[nodiscard]] unsigned int getRow() const
        {
            return row;
        }
        [[nodiscard]] unsigned int getColumn() const
        {
            return column;
        }

        void setRow(unsigned int row)
        {
            this->row = row;
        }

        void setColumn(unsigned int column)
        {
            this->column = column;
        }

        void update(unsigned int row, unsigned int col)
        {
            this->row    = row;
            this->column = col;
        }

        void reset()
        {
            update(0, 0);
        }

        [[nodiscard]] auto atTextBegin() const -> bool
        {
            return row == 0 && column == 0;
        }

        /// TODO make one from cursorMove - need to create TextModel first and establish boundaries
        bool move(const NavigationDirection &direction, std::unique_ptr<TextDocument> &document, TextType type);
    };
} // namespace gui
