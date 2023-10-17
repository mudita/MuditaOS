// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MultiTextLine.hpp"

namespace gui
{
    /// Note - line breaking could be done here with different TextLines to return
    /// or via different block types (i.e. numeric block tyle could be not "breakable"
    MultiTextLine::MultiTextLine(BlockCursor &localCursor, unsigned int maxWidth) : TextLine(maxWidth)
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

    unsigned int MultiTextLine::calculateSignsToShow(BlockCursor &localCursor, UTF8 &text, unsigned int space)
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

    UTF8 MultiTextLine::textToPrint(unsigned int signsCountToShow, UTF8 &text)
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

    MultiTextLine::MultiTextLine(MultiTextLine &&from) noexcept : TextLine(std::move(from))
    {
        breakLineDashAddition = from.breakLineDashAddition;
        removeTrailingSpace   = from.removeTrailingSpace;
    }
} // namespace gui
