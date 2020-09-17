#include "TextLine.hpp"
#include "Common.hpp"
#include "Label.hpp"
#include "TextBlock.hpp"
#include "TextDocument.hpp"
#include <cstdio>
#include <RawFont.hpp>

namespace gui
{

    /// helper function to get our text representation
    Label *buildUITextPart(const UTF8 &text, const TextFormat *format)
    {
        auto item = new gui::Label(nullptr);
        item->setText(text);
        item->setFont(format->getFont());
        item->setTextColor(format->getColor());
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
                break;
            }
            if (old_cursor.getBlockNr() != cursor.getBlockNr() &&
                (*document)(old_cursor).getEnd() == TextBlock::End::Newline) {
                end = TextBlock::End::Newline;
                break;
            }

            // it would be better if cursor would know what to do when it jumps over blocks

            // take Part of TextBlock we want to show
            auto text_part   = document->getTextPart(cursor);
            auto text_format = (*cursor).getFormat();
            if (text_format->getFont() == nullptr) {
                break;
            }
            auto can_show = text_format->getFont()->getCharCountInSpace(text_part.text, max_width - width_used);

            // we can show nothing - this is the end of this line
            if (can_show == 0) {
                auto item = buildUITextPart("", text_format);
                width_used += item->getTextNeedSpace();
                height_used = std::max(height_used, item->getTextHeight());

                elements_to_show_in_line.emplace_back(item);
                block_nr = cursor.getBlockNr();
                break;
            }

            // create item for show and update Line data
            auto item = buildUITextPart(text_part.text.substr(0, can_show), text_format);
            number_letters_shown += can_show;
            width_used += item->getTextNeedSpace();
            height_used = std::max(height_used, item->getTextHeight());
            elements_to_show_in_line.emplace_back(item);

            block_nr = cursor.getBlockNr();

            // not whole text shown, try again for next line if you want
            if (can_show < text_part.text.length()) {
                break;
            }

            old_cursor = cursor;
            cursor     = document->getBlockCursor(start_position + number_letters_shown);
        } while (true);
    }

    TextLine::TextLine(TextDocument *document,
                       unsigned int start_position,
                       unsigned int max_width,
                       unsigned int init_height,
                       bool drawUnderline,
                       UnderlineDrawMode drawUnderlineMode,
                       Position underlinePadding)
        : TextLine(document, start_position, max_width)
    {
        this->drawUnderline     = drawUnderline;
        this->drawUnderlineMode = drawUnderlineMode;
        this->underlinePadding  = underlinePadding;

        createUnderline(max_width, init_height);
    }

    TextLine::TextLine(TextLine &&from)
    {
        elements_to_show_in_line = std::move(from.elements_to_show_in_line);
        number_letters_shown     = from.number_letters_shown;
        width_used               = from.width_used;
        height_used              = from.height_used;
        underline                = from.underline;
        drawUnderline            = from.drawUnderline;
        drawUnderlineMode        = from.drawUnderlineMode;
        underlinePadding         = from.underlinePadding;
        end                      = from.end;
        block_nr                 = from.block_nr;
    }

    TextLine::~TextLine()
    {
        for (auto &el : elements_to_show_in_line) {
            if (el->parent == nullptr) {
                delete el;
            }
        }

        if (underline != nullptr && underline->parent == nullptr) {
            delete underline;
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

    void TextLine::setPosition(const short &x, const short &y)
    {
        auto line_x_position = x;

        updateUnderline(x, y);

        for (auto &el : elements_to_show_in_line) {
            auto scoped_disown = ScopedParentDisown(el);
            el->setArea({line_x_position, y - underlinePadding, el->getWidth(), el->getHeight()});
            line_x_position += el->getWidth();
        }
    }

    void TextLine::setParent(Item *parent)
    {
        if (parent == nullptr) {
            return;
        }

        parent->addWidget(underline);

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

        if (underline != nullptr && underline->parent != nullptr) {
            auto p = underline->parent;
            p->removeWidget(underline);
        }

        elements_to_show_in_line.clear();
    }

    void TextLine::alignH(Alignment line_align, Length parent_length) const
    {
        Position xOffset = line_align.calculateHAlignment(parent_length, getWidth());

        if (xOffset >= 0) {

            if (underline != nullptr && drawUnderlineMode == UnderlineDrawMode::Concurrent)
                underline->setPosition(underline->getPosition(Axis::X) + xOffset, Axis::X);

            for (auto &el : elements_to_show_in_line) {
                auto scoped_disown = ScopedParentDisown(el);
                el->setPosition(el->getPosition(Axis::X) + xOffset, Axis::X);
            }
        }
    }

    void TextLine::alignV(Alignment line_align, Length parent_length, Length lines_height)
    {
        Position yOffset = line_align.calculateVAlignment(parent_length, lines_height);

        if (yOffset >= 0 && yOffset != storedYOffset) {

            // Refactor - workaround for multiple offset addition.
            storedYOffset = yOffset;

            if (underline != nullptr)
                underline->setPosition(underline->getPosition(Axis::Y) + yOffset, Axis::Y);

            for (auto &el : elements_to_show_in_line) {
                auto scoped_disown = ScopedParentDisown(el);
                el->setPosition(el->getPosition(Axis::Y) + yOffset, Axis::Y);
            }
        }
    }

    void TextLine::createUnderline(unsigned int max_width, unsigned int init_height)
    {
        if (drawUnderline) {

            underline = new Rect(nullptr, 0, 0, max_width, init_height);
            underline->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);

            if (drawUnderlineMode == UnderlineDrawMode::WholeLine) {
                height_used = std::max(height_used, (Length)init_height);
            }
        }
    }

    void TextLine::updateUnderline(const short &x, const short &y)
    {
        if (underline != nullptr && drawUnderlineMode == UnderlineDrawMode::WholeLine) {
            underline->setArea({x, y, underline->widgetArea.w, height()});
        }
        else if (underline != nullptr && drawUnderlineMode == UnderlineDrawMode::Concurrent) {
            underline->setArea({x, y, getWidth(), height()});
        }
    }

} // namespace gui
