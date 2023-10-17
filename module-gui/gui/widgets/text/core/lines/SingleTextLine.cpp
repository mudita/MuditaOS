// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SingleTextLine.hpp"

namespace gui
{
    SingleTextLine::SingleTextLine(BlockCursor &localCursor, unsigned int maxWidth, TextEllipsis ellipsis)
        : TextLine(maxWidth), ellipsis(ellipsis)
    {
        do {
            if (!localCursor) { // cursor is faulty
                lineEnd = true;
                return;
            }

            if (localCursor.atEnd()) {
                lineEnd = true;
                return;
            }

            // take text we want to show
            auto text = localCursor.getUTF8Text();

            if (text.length() == 0 && !localCursor.atEnd()) {
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

            // create item for show and update Line data
            auto item = buildUITextPart(textToPrint(signsCountToShow, text), textFormat);
            shownLetterCount += signsCountToShow;
            widthUsed += item->widgetArea.w;
            heightUsed = std::max(heightUsed, item->widgetArea.h);
            lineContent.emplace_back(item);

            setLineStartConditions(localCursor.getBlockNumber(), localCursor.getPosition());
            localCursor += signsCountToShow;

            if (signsCountToShow < text.length()) {
                lineEnd = true;
                return;
            }

        } while (true);
    }

    bool SingleTextLine::checkLeftEndCondition(BlockCursor &localCursor)
    {
        return localCursor.getBlockNumber() != 0 || localCursor.getPosition() != 0;
    }

    bool SingleTextLine::checkRightEndCondition(BlockCursor &localCursor, unsigned int signsCountToShow)
    {
        BlockCursor moveCursor(localCursor.getDocument(),
                               localCursor.getPosition(),
                               localCursor.getBlockNumber(),
                               localCursor->getFormat()->getFont());
        moveCursor += signsCountToShow;

        return !moveCursor.atEnd();
    }

    // This method need correction - it does not handle block properly but there is no use case in system to
    // handle it for now.
    unsigned int SingleTextLine::calculateSignsToShow(BlockCursor &localCursor, UTF8 &text, unsigned int space)
    {
        auto textFormat       = localCursor->getFormat();
        auto signsCountToShow = textFormat->getFont()->getCharCountInSpace(text, space);

        auto leftCondition  = checkLeftEndCondition(localCursor);
        auto rightCondition = checkRightEndCondition(localCursor, signsCountToShow);

        if (ellipsis == TextEllipsis::Left && leftCondition) {
            drawnEllipsis = TextEllipsis::Left;
        }
        else if (ellipsis == TextEllipsis::Right && rightCondition) {
            drawnEllipsis = TextEllipsis::Right;
        }
        else if (ellipsis == TextEllipsis::Both) {
            if (!leftCondition && !rightCondition) {
                drawnEllipsis = TextEllipsis::None;
            }
            else if (leftCondition || rightCondition) {
                drawnEllipsis = TextEllipsis::Left;
                signsCountToShow =
                    textFormat->getFont()->getCharCountInSpace(text, space - calculateEllipsisWidth(textFormat));
                rightCondition = checkRightEndCondition(localCursor, signsCountToShow);

                if (leftCondition && !rightCondition) {
                    drawnEllipsis = TextEllipsis::Left;
                }
                else if (!leftCondition && rightCondition) {
                    drawnEllipsis = TextEllipsis::Right;
                }
                else if (leftCondition && rightCondition) {
                    drawnEllipsis = TextEllipsis::Both;
                }
            }
        }

        signsCountToShow = textFormat->getFont()->getCharCountInSpace(text, space - calculateEllipsisWidth(textFormat));

        return signsCountToShow;
    }

    unsigned int SingleTextLine::calculateEllipsisWidth(const TextFormat *format)
    {
        switch (drawnEllipsis) {
        case TextEllipsis::Left:
        case TextEllipsis::Right:
            return format->getFont()->getPixelWidth(text::ellipsis_signs);
        case TextEllipsis::Both:
            return 2 * format->getFont()->getPixelWidth(text::ellipsis_signs);
        default:
            return 0;
        }
    }

    UTF8 SingleTextLine::textToPrint(unsigned int signsCountToShow, UTF8 &text)
    {
        auto textToPrint = text.substr(0, signsCountToShow);

        switch (drawnEllipsis) {
        case TextEllipsis::Left:
            return UTF8(text::ellipsis_signs) + textToPrint;
        case TextEllipsis::Right:
            return textToPrint + UTF8(text::ellipsis_signs);
        case TextEllipsis::Both:
            return UTF8(text::ellipsis_signs) + textToPrint + UTF8(text::ellipsis_signs);
        default:
            return textToPrint;
        }
    }

    SingleTextLine::SingleTextLine(SingleTextLine &&from) noexcept : TextLine(std::move(from))
    {
        ellipsis = from.ellipsis;
    }

} // namespace gui
