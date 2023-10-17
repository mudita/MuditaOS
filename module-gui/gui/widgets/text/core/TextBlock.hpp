// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <TextConstants.hpp>
#include <core/TextFormat.hpp>
#include <utf8/UTF8.hpp>

namespace gui
{
    class RawFont;
    /// single text block. It might end with formatting, or with newline.
    /// it is responsible for:
    /// * storing part of text
    /// * storing properties of stored text
    class TextBlock
    {
        std::unique_ptr<TextFormat> format = nullptr;
        UTF8 text;

      public:
        enum class End
        {
            Newline,
            None
        };

      private:
        End end = End::None;

      public:
        TextBlock(const UTF8 text, const RawFont *font, End eol = End::None);
        TextBlock(const UTF8 text, std::unique_ptr<TextFormat> format);
        TextBlock(const TextBlock &);

        const UTF8 &getText() const;
        UTF8 getText(uint32_t start_position) const;
        auto getFormat() const -> const TextFormat *;
        void setText(const UTF8 text);
        void insertChar(const uint32_t value, const uint32_t pos);
        void removeChar(const uint32_t pos);

        uint32_t getWidth() const;
        unsigned int length() const;
        auto getEnd() const -> End;
        void setEnd(End end);

        void addChar(uint32_t utf_val, unsigned int pos);
        bool isEmpty() const;

        TextBlock &operator=(const TextBlock &);
    };
} // namespace gui
