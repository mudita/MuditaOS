#include "TextBlock.hpp"
#include "TextConstants.hpp"
#include "log/log.hpp"
#include <cassert>
#include <TextFormat.hpp>
#include <RawFont.hpp>

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

    void TextBlock::setEnd(End end)
    {
        if (getEnd() != End::Newline) {
            text.insertCode(text::newline);
        }
        this->end = end;
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
