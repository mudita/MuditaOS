// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TextLine.hpp"

namespace gui
{

    /// helper function to get our text representation
    RawText *TextLine::buildUITextPart(const UTF8 &text, const TextFormat *format)
    {
        auto item = new gui::RawText(text, format->getFont(), format->getColor());
        return item;
    }

    TextLine::TextLine(TextLine &&from) noexcept
    {
        lineContent            = std::move(from.lineContent);
        shownLetterCount       = from.shownLetterCount;
        widthUsed              = from.widthUsed;
        heightUsed             = from.heightUsed;
        underline              = from.underline;
        underLineProperties    = from.underLineProperties;
        lineEnd                = from.lineEnd;
        end                    = from.end;
        maxWidth               = from.maxWidth;
        lineStartBlockNumber   = from.lineStartBlockNumber;
        lineStartBlockPosition = from.lineStartBlockPosition;
        lineVisible            = from.lineVisible;
    }

    TextLine::~TextLine()
    {
        for (auto &el : lineContent) {
            if (el->parent == nullptr) {
                delete el;
            }
        }

        if (underline != nullptr && underline->parent == nullptr) {
            delete underline;
        }
    }

    void TextLine::setPosition(const short &x, const short &y)
    {
        auto lineXPosition = x;

        updateUnderline(x, y);

        for (auto &el : lineContent) {
            auto scopedDisown = ScopedParentDisown(el);
            el->setArea({lineXPosition, y - underLineProperties.underLinePadding, el->getWidth(), el->getHeight()});
            lineXPosition += el->getWidth();
        }
    }

    void TextLine::setLineStartConditions(unsigned int startBlockNumber, unsigned int startBlockPosition)
    {
        if (lineStartBlockNumber == text::npos && lineStartBlockPosition == text::npos) {
            lineStartBlockNumber   = startBlockNumber;
            lineStartBlockPosition = startBlockPosition;
        }
    }

    void TextLine::setParent(Item *parent)
    {
        if (parent == nullptr) {
            return;
        }

        createUnderline(maxWidth, underLineProperties.underlineHeight);
        parent->addWidget(underline);

        for (auto &el : lineContent) {
            parent->addWidget(el);
        }
    }

    Length TextLine::getWidth() const
    {
        Length width = 0;
        for (auto &line : lineContent) {
            width += line->getWidth();
        }
        return width;
    }

    uint32_t TextLine::getWidthTo(unsigned int pos) const
    {
        uint32_t width  = 0;
        auto currentPos = 0;
        if (pos == text::npos) {
            return 0;
        }
        for (auto &el : lineContent) {
            if (el->getFont() == nullptr) {
                continue;
            }
            if (currentPos + el->getTextLength() > pos) {
                width += el->getFont()->getPixelWidth(el->getText(), 0, pos - currentPos);
                return width;
            }
            else {
                width += el->getWidth();
            }
            currentPos += el->getTextLength();
        }
        return width;
    }

    const Item *TextLine::getElement(unsigned int pos) const noexcept
    {
        unsigned int local_pos = 0;
        for (auto &el : lineContent) {
            local_pos += el->getTextLength();
            if (local_pos >= pos) {
                return el;
            }
        }
        return nullptr;
    }

    UTF8 TextLine::getText(unsigned int pos) const
    {
        UTF8 text;
        for (auto &label : lineContent) {
            if (label->getFont() == nullptr) {
                continue;
            }
            text += label->getText();
        }

        return text;
    }

    void TextLine::erase()
    {
        for (auto &el : lineContent) {
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

        lineContent.clear();
    }

    void TextLine::alignH(Alignment lineAlign, Length parentLength) const
    {
        Position xOffset = lineAlign.calculateHAlignment(parentLength, getWidth());

        if (xOffset >= 0) {

            if (underline != nullptr && underLineProperties.drawUnderlineMode == UnderlineDrawMode::Concurrent)
                underline->setPosition(underline->getPosition(Axis::X) + xOffset, Axis::X);

            for (auto &el : lineContent) {
                auto scopedDisown = ScopedParentDisown(el);
                el->setPosition(el->getPosition(Axis::X) + xOffset, Axis::X);
            }
        }
    }

    void TextLine::alignV(Alignment lineAlign, Length parentLength, Length linesHeight)
    {
        Position yOffset = lineAlign.calculateVAlignment(parentLength, linesHeight);

        if (yOffset >= 0 && yOffset != storedYOffset) {

            // Refactor - workaround for multiple offset addition.
            storedYOffset = yOffset;

            if (underline != nullptr)
                underline->setPosition(underline->getPosition(Axis::Y) + yOffset, Axis::Y);

            for (auto &el : lineContent) {
                auto scopedDisown = ScopedParentDisown(el);
                el->setPosition(el->getPosition(Axis::Y) + yOffset, Axis::Y);
            }
        }
    }

    void TextLine::createUnderline(unsigned int width, unsigned int initHeight)
    {
        if (underLineProperties.draw) {

            underline = new Rect(nullptr, 0, 0, maxWidth, initHeight);
            underline->setPenWidth(underLineProperties.thickness);
            underline->setEdges(RectangleEdge::Bottom);

            if (underLineProperties.drawUnderlineMode == UnderlineDrawMode::WholeLine) {
                heightUsed = std::max(heightUsed, (Length)initHeight);
            }
        }
    }

    void TextLine::updateUnderline(const short &x, const short &y)
    {
        if (underline != nullptr && underLineProperties.drawUnderlineMode == UnderlineDrawMode::WholeLine) {
            underline->setArea({x, y, underline->widgetArea.w, height()});
        }
        else if (underline != nullptr && underLineProperties.drawUnderlineMode == UnderlineDrawMode::Concurrent) {
            underline->setArea({x, y, getWidth(), height()});
        }
    }
} // namespace gui
