// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "TextLine.hpp"

namespace gui
{
    class SingleTextLine : public TextLine
    {
      private:
        TextEllipsis ellipsis = TextEllipsis::None;
        unsigned int calculateSignsToShow(BlockCursor &localCursor, UTF8 &text, unsigned int space);
        unsigned int calculateEllipsisWidth(const TextFormat *format);
        UTF8 textToPrint(unsigned int signsCountToShow, UTF8 &text);
        bool checkLeftEndCondition(BlockCursor &localCursor);
        bool checkRightEndCondition(BlockCursor &localCursor, unsigned int signsCountToShow);

      public:
        SingleTextLine(BlockCursor &, unsigned int maxWidth, TextEllipsis ellipsis);
        SingleTextLine(SingleTextLine &) = delete;
        SingleTextLine(SingleTextLine &&) noexcept;

        SingleTextLine(BlockCursor &cursor,
                       unsigned int maxWidth,
                       unsigned int initHeight,
                       UnderLineProperties underLineProperties,
                       TextEllipsis ellipsis)
            : SingleTextLine(cursor, maxWidth, ellipsis)
        {
            this->underLineProperties                 = underLineProperties;
            this->underLineProperties.underlineHeight = initHeight;
        }

        ~SingleTextLine() = default;
    };
} // namespace gui
