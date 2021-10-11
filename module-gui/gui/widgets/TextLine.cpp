// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "TextLine.hpp"
#include "Common.hpp"
#include "TextBlock.hpp"
#include "TextDocument.hpp"
#include "TextCursor.hpp"
#include <RawFont.hpp>
#include <numeric>

namespace gui
{

    /// helper function to get our text representation
    RawText *buildUITextPart(const UTF8 &text, const TextFormat *format)
    {
        auto item = new gui::RawText(text, format->getFont(), format->getColor());
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

            auto signsCountToShow = calculateSignsToShow(localCursor, text, maxWidth - widthUsed);

            // we can show nothing - this is the end of this line
            if (signsCountToShow == 0) {
                auto item  = buildUITextPart("", textFormat);
                widthUsed  = item->widgetArea.w;
                heightUsed = std::max(heightUsed, item->widgetArea.h);
                lineContent.emplace_back(item);
                end = TextBlock::End::None;

                setLineStartConditions(localCursor.getBlockNumber(), localCursor.getPosition());

                return;
            }

            // create item for show and update Line data
            auto item = buildUITextPart(textToPrint(signsCountToShow, text), textFormat);
            shownLetterCount += signsCountToShow;
            widthUsed += item->widgetArea.w;
            heightUsed = std::max(heightUsed, item->widgetArea.h);
            lineContent.emplace_back(item);

            setLineStartConditions(localCursor.getBlockNumber(), localCursor.getPosition());

            localCursor += signsCountToShow;

            if (removeTrailingSpace) {
                end = TextBlock::End::Newline;
                return;
            }

            if (localCursor.checkAndInvalidateBlockChanged() && localCursor.checkPreviousBlockNewLine()) {
                end = TextBlock::End::Newline;
                return;
            }

            // not whole text shown, try again for next line if you want
            if (signsCountToShow < text.length()) {
                end = TextBlock::End::None;
                return;
            }

        } while (true);
    }

    unsigned int TextLine::calculateSignsToShow(BlockCursor &localCursor, UTF8 &text, unsigned int space)
    {
        auto signsCountToShow = localCursor->getFormat()->getFont()->getCharCountInSpace(text, space);

        // additional one sign to detect potential space as last character in line
        auto searchSubstring = text.substr(0, signsCountToShow + 1);

        auto newlinePos = searchSubstring.findLast("\n", signsCountToShow);
        auto spacePos   = searchSubstring.findLast(" ", signsCountToShow);

        // check if space or newline in word detection range
        if (spacePos <= signsCountToShow - text::word_detection_range) {
            spacePos = UTF8::npos;
        }

        if (newlinePos <= signsCountToShow - text::word_detection_range) {
            newlinePos = UTF8::npos;
        }

        // check if space found and no newline at end
        if (spacePos != UTF8::npos && newlinePos == UTF8::npos) {
            // if line ends on space remove it when drawing
            if (spacePos >= signsCountToShow) {
                removeTrailingSpace = true;
            }

            // add one to include space in the line
            signsCountToShow = spacePos + 1;

            breakLineDashAddition = false;
        }
        // if sings still left in text add dash sign
        else if (signsCountToShow != 0 && signsCountToShow < text.length()) {
            // decrease character shown count by one to fit dash
            signsCountToShow      = signsCountToShow - 1;
            breakLineDashAddition = true;
        }

        return signsCountToShow;
    }

    UTF8 TextLine::textToPrint(unsigned int signsCountToShow, UTF8 &text)
    {
        auto textToPrint = text.substr(0, signsCountToShow);

        if (removeTrailingSpace) {
            textToPrint = text.substr(0, signsCountToShow - 1);
        }

        if (breakLineDashAddition) {
            textToPrint = textToPrint + "-";
        }

        return textToPrint;
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
        breakLineDashAddition  = from.breakLineDashAddition;
        removeTrailingSpace    = from.removeTrailingSpace;
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
