// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <list>
#include "Common.hpp"
#include "Label.hpp"
#include "TextDocument.hpp"
#include "BoxLayout.hpp"
#include "TextLineCursor.hpp"

namespace gui
{
    class TextCursor;

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
        Length max_width                  = 0;
        std::list<Label *> elements_to_show_in_line;
        Rect *underline                     = nullptr;
        bool drawUnderline                  = false;
        UnderlineDrawMode drawUnderlineMode = UnderlineDrawMode::Concurrent;
        Position underlinePadding           = 0;
        TextBlock::End end                  = TextBlock::End::None;
        Position storedYOffset              = 0;

        void createUnderline(unsigned int max_w, unsigned int max_height);
        void updateUnderline(const short &x, const short &y);

      public:
        /// creates TextLine with data from text based on TextCursor position filling max_width
        TextLine(BlockCursor &, unsigned int max_width);
        TextLine(TextLine &) = delete;
        TextLine(TextLine &&);

        TextLine(BlockCursor &cursor,
                 unsigned int max_width,
                 unsigned int init_height,
                 bool drawUnderline,
                 UnderlineDrawMode drawUnderlineMode,
                 Position underlinePadding)
            : TextLine(cursor, max_width)
        {
            this->drawUnderline     = drawUnderline;
            this->drawUnderlineMode = drawUnderlineMode;
            this->underlinePadding  = underlinePadding;

            createUnderline(max_width, init_height);
        }

        ~TextLine();

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
                LOG_INFO("Wchodzisz tutaj w ogole?");
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
        auto getText(unsigned int pos) const -> UTF8;
        auto checkBounds(TextLineCursor &cursor, uint32_t utf_value, const TextFormat *format) -> InputBound;
    };
} // namespace gui
