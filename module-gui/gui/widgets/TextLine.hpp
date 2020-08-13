#pragma once

#include <list>
#include "Common.hpp"
#include "Label.hpp"
#include "TextDocument.hpp"
#include "BoxLayout.hpp"

namespace gui
{
    enum class UnderlineDrawMode
    {
        WholeLine,
        Concurrent
    };

    /// interface element for TextDocument->getLine() <-- Text
    class TextLine
    {
        unsigned int number_letters_shown = 0;
        Length width_used                 = 0;
        Length height_used                = 0;
        std::list<Label *> elements_to_show_in_line;
        Rect *underline                     = nullptr;
        bool drawUnderline                  = false;
        UnderlineDrawMode drawUnderlineMode = UnderlineDrawMode::Concurrent;
        Position underlinePadding           = 0;
        TextBlock::End end                  = TextBlock::End::None;
        unsigned int block_nr               = text::npos;
        Position storedYOffset              = 0;

        void createUnderline(unsigned int max_width, unsigned int max_height);
        void updateUnderline(const short &x, const short &y);

      public:
        /// creates TextLine with data from text from start position in `TextDocument` filling max_width
        /// @note might be better to have TextBlockIterator which could hop through TextBlock inside TextDocument
        TextLine(TextDocument *, unsigned int start_position, unsigned int max_width);
        TextLine(TextDocument *,
                 unsigned int start_position,
                 unsigned int max_width,
                 unsigned int init_height,
                 bool drawUnderline,
                 UnderlineDrawMode drawUnderlineMode,
                 Position underlinePadding = 0);
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

        TextBlock::End getEnd() const
        {
            return end;
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

        unsigned int getBlockNr() const
        {
            return block_nr;
        }

        void setPosition(const short &x, const short &y);
        void setParent(Item *parent);
        [[nodiscard]] Length getWidth() const;
        [[nodiscard]] Length getWidthTo(unsigned int pos) const;
        void erase();
        /// align TextLine due to alignment axis in parent_width
        ///
        /// moves Text parts in Text. To not call n times callbacks on resize, call prior to setting parent
        void alignH(Alignment align, Length parent_length) const;
        void alignV(Alignment align, Length parent_length, Length lines_height);
    };
} // namespace gui
