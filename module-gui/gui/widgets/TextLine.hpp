#pragma once

#include <list>
#include "Common.hpp"
#include "Label.hpp"
#include "TextDocument.hpp"

namespace gui
{

    /// interface element for TextDocument->getLine() <-- Text
    class TextLine
    {
        unsigned int number_letters_shown = 0;
        Length width_used                 = 0;
        Length height_used                = 0;
        std::list<Label *> elements_to_show_in_line;

      public:
        /// creates TextLine with data from text from start position in `TextDocument` filling max_width
        /// @note might be better to have TextBlockIterator which could hop through TextBlock inside TextDocument
        TextLine(TextDocument *, unsigned int start_position, unsigned int max_width);
        ~TextLine();
        TextLine(TextLine &) = delete;
        TextLine(TextLine &&);

        /// number of letters in Whole TextLines
        unsigned int length() const
        {
            return number_letters_shown;
        }

        /// count of elements in whole TextLine
        unsigned int count() const
        {
            return elements_to_show_in_line.size();
        }

        Length width() const
        {
            return width_used;
        }

        Length height() const
        {
            return height_used;
        }

        const Item *getElement(unsigned int pos) const
        {
            unsigned int local_pos = 0;
            for (auto &el : elements_to_show_in_line) {
                local_pos += el->getTextLength();
                if (local_pos >= pos) {
                    return el;
                }
            }
            return nullptr;
        }

        int32_t getX() const
        {
            return elements_to_show_in_line.front()->area().pos(Axis::X);
        }

        void setPosition(int32_t x, int32_t y);
        void setParent(Item *parent);
        Length getWidth() const;
        Length getWidthTo(unsigned int pos) const;
        void erase();
        /// align TextLine due to alignment axis in parent_width
        ///
        /// moves Text parts in Text. To not call n times callbacks on resize, call prior to setting parent
        void align(Alignment align, Length parent_length);
    };
} // namespace gui
