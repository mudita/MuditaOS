// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "TextCursor.hpp"

namespace gui
{
    class Text;
    class TextDocument;

    class TextInLineCursor : public TextCursor
    {
      protected:
        bool checkInLineRightMove();
        bool checkInLineLeftMove();

        void moveInLineLeft();
        void moveInLineRight();

        unsigned int calculateMinWidthToMoveWithEllipsis(RawFont *font);

      public:
        explicit TextInLineCursor(gui::Text *parent, unsigned int pos = text::npos, unsigned int block = text::npos);
        TextInLineCursor() = delete;

        auto moveCursor(NavigationDirection direction) -> Move override;
        auto moveCursor(NavigationDirection direction, unsigned int n) -> Move override;

        bool removeChar() final;
    };

} // namespace gui
