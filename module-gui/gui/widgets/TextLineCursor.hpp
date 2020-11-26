// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "TextCursor.hpp"

namespace gui
{
    class Text;
    class TextDocument;

    class TextLineCursor : public TextCursor
    {
      protected:
      public:
        explicit TextLineCursor(gui::Text *parent, unsigned int pos = text::npos, unsigned int block = text::npos);
        TextLineCursor() = delete;

        auto moveCursor(NavigationDirection direction) -> Move override;
    };

} // namespace gui
