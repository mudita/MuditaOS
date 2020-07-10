#pragma once

#include "Color.hpp"
#include "Font.hpp"
#include "TextConstants.hpp"
#include "utf8/UTF8.hpp"

namespace gui
{
    /// single text block. It might end with formatting, or with newline.
    /// it is responsible for:
    /// * storing part of text
    /// * storing properties of stored text
    class TextBlock
    {
        Font *font = nullptr;
        UTF8 text;
        Color color = ColorFullBlack;

      public:
        enum class End
        {
            Newline,
            None
        };

      private:
        End end = End::Newline;

      public:
        TextBlock(const UTF8 text, Font *font, End eol = End::None);

        const UTF8 &getText() const;
        UTF8 getText(uint32_t start_position) const;
        Font *getFont() const;
        void setText(const UTF8 text);
        void insertChar(const uint32_t value, const uint32_t pos);
        void removeChar(const uint32_t pos);

        uint32_t getWidth() const;
        unsigned int length() const;
        auto getEnd() const -> End;
        void setEnd(End end);

        void addChar(uint32_t utf_val, unsigned int pos);
        bool isEmpty() const;
    };
} // namespace gui
