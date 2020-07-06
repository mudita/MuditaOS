#include "TextLine.hpp"
#include "Common.hpp"
#include "Label.hpp"
#include "TextBlock.hpp"
#include "TextDocument.hpp"
#include <cstdio>

namespace gui
{

    /// helper function to get our text representation
    Label *buildUITextPart(const UTF8 &text, Font *font)
    {
        auto item = new gui::Label(nullptr);
        item->setText(text);
        item->setFont(font);
        item->setSize(item->getTextNeedSpace(), item->getTextHeight());
        item->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        return item;
    }

    /// Note - line breaking could be done here with different TextLines to return
    /// or via different block types (i.e. numeric block tyle could be not "breakable"
    TextLine::TextLine(TextDocument *document, unsigned int start_position, unsigned int max_width)
    {
        if (document == nullptr) {
            return;
        }
        auto cursor     = document->getBlockCursor(start_position);
        auto old_cursor = cursor;
        do {
            if (!cursor) { // cursor is faulty
                return;
            }
            // it would be better if cursor would know what to do when it jumps over blocks
            if (old_cursor.getBlockNr() != cursor.getBlockNr() &&
                (*document)(old_cursor).getEnd() == TextBlock::End::Newline) {
                return;
            }

            // take Part of TextBlock we want to show
            auto text_part = document->getTextPart(cursor);
            if (text_part.font == nullptr) {
                return;
            }
            auto can_show = text_part.font->getCharCountInSpace(text_part.text, max_width - width_used);

            // we can show nothing - this is the end of this line
            if (can_show == 0) {
                return;
            }

            // create item for show and update Line data
            auto item = buildUITextPart(text_part.text.substr(0, can_show), text_part.font);
            number_letters_shown += can_show;
            width_used += item->getTextNeedSpace();
            height_used = std::max(height_used, item->getTextHeight());
            elements_to_show_in_line.emplace_back(item);

            // not whole text shown, try again for next line if you want
            if (can_show < text_part.text.length()) {
                return;
            }

            old_cursor = cursor;
            cursor     = document->getBlockCursor(start_position + number_letters_shown);
        } while (true);
    }

    TextLine::TextLine(TextLine &&from)
    {
        elements_to_show_in_line = std::move(from.elements_to_show_in_line);
        number_letters_shown     = from.number_letters_shown;
        width_used               = from.width_used;
        height_used              = from.height_used;
    }

    TextLine::~TextLine()
    {
        for (auto &el : elements_to_show_in_line) {
            if (el->parent == nullptr) {
                delete el;
            }
        }
    }

    /// class to disown Item temporary to ignore callback
    class ScopedParentDisown
    {
        Item *parent = nullptr;
        Item *item   = nullptr;

      public:
        ScopedParentDisown(Item *it) : item(it)
        {
            if (item != nullptr) {
                parent = item->parent;
            }
        }

        ~ScopedParentDisown()
        {
            if (item != nullptr) {
                item->parent = parent;
            }
        }
    };

    void TextLine::setPosition(int32_t x, int32_t y)
    {
        auto line_x_position = x;
        for (auto &el : elements_to_show_in_line) {
            auto scoped_disown          = ScopedParentDisown(el);
            int32_t align_bottom_offset = height() - el->getHeight();
            el->setArea({line_x_position, y + align_bottom_offset, el->getWidth(), el->getHeight()});
            line_x_position += el->getWidth();
        }
    }

    void TextLine::setParent(Item *parent)
    {
        if (parent == nullptr) {
            return;
        }
        for (auto &el : elements_to_show_in_line) {
            parent->addWidget(el);
        }
    }

    Length TextLine::getWidth() const
    {
        Length width = 0;
        for (auto &line : elements_to_show_in_line) {
            width += line->getWidth();
        }
        return width;
    }

    uint32_t TextLine::getWidthTo(unsigned int pos) const
    {
        uint32_t width  = 0;
        auto curent_pos = 0;
        if (pos == text::npos) {
            return 0;
        }
        for (auto &el : elements_to_show_in_line) {
            if (el->getFont() == nullptr) {
                continue;
            }
            if (curent_pos + el->getTextLength() > pos) {
                width += el->getFont()->getPixelWidth(el->getText(), 0, pos - curent_pos);
                return width;
            }
            else {
                width += el->getWidth();
            }
            curent_pos += el->getTextLength();
        }
        return width;
    }

    void TextLine::erase()
    {
        for (auto &el : elements_to_show_in_line) {
            if (el->parent != nullptr) {
                auto p = el->parent;
                p->erase(el);
            }
            else {
                delete el;
            }
        }
        elements_to_show_in_line.clear();
    }

    void TextLine::align(Alignment line_align, Length parent_length)
    {
        Length width = getWidth();
        if (parent_length <= width) {
            return;
        }
        Length offset = 0;
        if (line_align.horizontal_right) {
            offset = parent_length - width;
        }
        else if (line_align.horizontal_center) {
            offset = (parent_length - width) / 2;
        }
        for (auto &el : elements_to_show_in_line) {
            auto scoped_disown = ScopedParentDisown(el);
            el->setPosition(el->getPosition(Axis::X) + offset, Axis::X);
        }
    }
} // namespace gui
