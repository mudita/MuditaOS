// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "TextLine.hpp"

namespace gui
{
    class MultiTextLine : public TextLine
    {
      private:
        bool breakLineDashAddition = false;
        bool removeTrailingSpace   = false;

        unsigned int calculateSignsToShow(BlockCursor &localCursor, UTF8 &text, unsigned int space);
        UTF8 textToPrint(unsigned int signsCountToShow, UTF8 &text);

      public:
        MultiTextLine(BlockCursor &, unsigned int maxWidth);
        MultiTextLine(MultiTextLine &) = delete;
        MultiTextLine(MultiTextLine &&) noexcept;

        MultiTextLine(BlockCursor &cursor,
                      unsigned int maxWidth,
                      unsigned int initHeight,
                      UnderLineProperties underLineProperties)
            : MultiTextLine(cursor, maxWidth)
        {
            this->underLineProperties                 = underLineProperties;
            this->underLineProperties.underlineHeight = initHeight;
        }

        ~MultiTextLine() = default;
    };
} // namespace gui
