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

        TextCursor(gui::Item *parent);
        [[nodiscard]] unsigned int getRow() const;
        [[nodiscard]] unsigned int getColumn() const;
        void setRow(unsigned int row);
        void setColumn(unsigned int column);
        void update(unsigned int row, unsigned int col);
        void reset();
        [[nodiscard]] auto atTextBegin() const -> bool;
        bool move(const NavigationDirection &direction, std::unique_ptr<TextDocument> &document, TextType type);
    };
} // namespace gui
