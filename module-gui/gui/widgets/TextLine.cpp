// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TextLine.hpp"
#include "Common.hpp"
#include "Label.hpp"
#include "TextBlock.hpp"
#include "TextDocument.hpp"
#include "TextCursor.hpp"
#include <cstdio>
#include <RawFont.hpp>
#include <numeric>

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
        item->setEdges(RectangleEdge::None);
        return item;
    }

    /// Note - line breaking could be done here with different TextLines to return
    /// or via different block types (i.e. numeric block tyle could be not "breakable"
    TextLine::TextLine(BlockCursor &localCursor, unsigned int maxWidth) : maxWidth(maxWidth)
    {
        do {
            if (!localCursor) { // cursor is faulty
                lineEnd = true;
                return;
            }

            if (localCursor.atEnd() && !localCursor.checkLastNewLine()) {
                lineEnd = true;
                return;
            }

            // take text we want to show
            auto text = localCursor.getUTF8Text();

            if (text.length() == 0 && localCursor.checkCurrentBlockNoNewLine() && !localCursor.atEnd()) {
                setLineStartConditions(localCursor.getBlockNumber(), localCursor.getPosition());
                localCursor.goToNextBlock();
                continue;
            }

            auto textFormat = localCursor->getFormat();
            if (textFormat->getFont() == nullptr) {
                lineEnd = true;
                return;
            }

            // check if max provided width is enough to enter one char at least
            if (maxWidth < textFormat->getFont()->getCharPixelWidth(text[0])) {
                lineEnd = true;
                return;
            }

            auto canShow = textFormat->getFont()->getCharCountInSpace(text, maxWidth - widthUsed);

            // we can show nothing - this is the end of this line
            if (canShow == 0) {

                auto item  = buildUITextPart("", textFormat);
                widthUsed  = item->getTextNeedSpace();
                heightUsed = std::max(heightUsed, item->getTextHeight());
                lineContent.emplace_back(item);
                end = TextBlock::End::None;

                setLineStartConditions(localCursor.getBlockNumber(), localCursor.getPosition());

                return;
            }

            // create item for show and update Line data
            auto item = buildUITextPart(text.substr(0, canShow), textFormat);
            shownLetterCount += canShow;
            widthUsed += item->getTextNeedSpace();
            heightUsed = std::max(heightUsed, item->getTextHeight());
            lineContent.emplace_back(item);

            setLineStartConditions(localCursor.getBlockNumber(), localCursor.getPosition());

            localCursor += canShow;

            if (localCursor.checkAndInvalidateBlockChanged() && localCursor.checkPreviousBlockNewLine()) {
                end = TextBlock::End::Newline;
                return;
            }

            // not whole text shown, try again for next line if you want
            if (canShow < text.length()) {
                end = TextBlock::End::None;
                return;
            }

        } while (true);
    }

    TextLine::TextLine(TextLine &&from) noexcept
    {
        lineContent            = std::move(from.lineContent);
        shownLetterCount       = from.shownLetterCount;
        widthUsed              = from.widthUsed;
        heightUsed             = from.heightUsed;
        underline              = from.underline;
        drawUnderline          = from.drawUnderline;
        drawUnderlineMode      = from.drawUnderlineMode;
        underlinePadding       = from.underlinePadding;
        underlineHeight        = from.underlineHeight;
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
        auto lineXPosition = x;

        updateUnderline(x, y);

        for (auto &el : lineContent) {
            auto scopedDisown = ScopedParentDisown(el);
            el->setArea({lineXPosition, y - underlinePadding, el->getWidth(), el->getHeight()});
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

        createUnderline(maxWidth, underlineHeight);
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

            if (underline != nullptr && drawUnderlineMode == UnderlineDrawMode::Concurrent)
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
        if (drawUnderline) {

            underline = new Rect(nullptr, 0, 0, maxWidth, initHeight);
            underline->setEdges(RectangleEdge::Bottom);

            if (drawUnderlineMode == UnderlineDrawMode::WholeLine) {
                heightUsed = std::max(heightUsed, (Length)initHeight);
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
