// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TextBlock.hpp"
#include <TextConstants.hpp>
#include <RawFont.hpp>

#include <log/log.hpp>

namespace gui
{

    TextBlock::TextBlock(const UTF8 text, std::unique_ptr<TextFormat> format) : format(std::move(format)), text{text}
    {}

    TextBlock::TextBlock(const UTF8 text, const RawFont *font, TextBlock::End eol)
        : TextBlock(text, std::make_unique<TextFormat>(font))
    {
        if (getEnd() != End::Newline && eol == End::Newline) {
            this->text.insertCode(text::newline);
        }
        end = eol;
    }

    TextBlock::TextBlock(const TextBlock &p)
    {
        text   = p.text;
        format = std::make_unique<TextFormat>(*p.format);
        end    = p.end;
    }

    TextBlock &TextBlock::operator=(const TextBlock &p)
    {
        if (this != &p) {
            text   = p.text;
            format = std::make_unique<TextFormat>(*p.format);
            end    = p.end;
        }
        return *this;
    }

    const UTF8 &TextBlock::getText() const
    {
        return text;
    }

    UTF8 TextBlock::getText(uint32_t start_position) const
    {
        return text.substr(start_position, text.length() - start_position);
    }

    const TextFormat *TextBlock::getFormat() const
    {
        return format.get();
    }

    void TextBlock::setText(const UTF8 text)
    {
        this->text = text;
    }

    /// sick there is no add/append in UTF8 - there is insert...
    void TextBlock::insertChar(const uint32_t value, const uint32_t pos)
    {
        text.insertCode(value, pos);
    }

    void TextBlock::removeChar(const uint32_t pos)
    {
        text.removeChar(pos);
    }

    uint32_t TextBlock::getWidth() const
    {
        auto font = format->getFont();
        if (font != nullptr) {
            return font->getPixelWidth(text);
        }
        return 0;
    }

    unsigned int TextBlock::length() const
    {
        return text.length();
    }

    TextBlock::End TextBlock::getEnd() const
    {
        if (text.length() > 0 && text[text.length() - 1] == text::newline) {
            return End::Newline;
        }
        return End::None;
    }

    void TextBlock::setEnd(End _end)
    {
        if (_end == End::Newline && getEnd() != End::Newline) {
            text.insertCode(text::newline);
        }
        else if (_end == End::None && getEnd() == End::Newline) {
            text.removeChar(text.length() - 1);
        }
        this->end = _end;
    }

    void TextBlock::addChar(uint32_t utf_val, unsigned int pos)
    {
        if (pos == text::npos) {
            pos = 0;
        }
        text.insertCode(utf_val, pos);
    }

    bool TextBlock::isEmpty() const
    {
        return text.length() == 0 && getEnd() == End::None;
    }

} // namespace gui
