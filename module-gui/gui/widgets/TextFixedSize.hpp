// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Text.hpp"

namespace gui
{

    class TextFixedSize : public Text
    {
      protected:
        unsigned int linesCount = style::text::maxTextLines;

        void drawLines() override;

      public:
        TextFixedSize();
        explicit TextFixedSize(Item *parent, Position x = 0U, Position y = 0U, Length w = 0U, Length h = 0U);

        void setLines(unsigned int val);
        void setLinesSpacing(unsigned int val);
        void setUnderlinePadding(Position val);
        void setUnderlineThickness(unsigned int val);
    };

} // namespace gui
